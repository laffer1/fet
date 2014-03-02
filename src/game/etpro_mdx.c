/*
                                                                                                            
  **    ***    ***            ***   ****                                   *               ***              
  **    ***    ***            ***  *****                                 ***               ***              
  ***  *****  ***             ***  ***                                   ***                                
  ***  *****  ***    *****    *** ******   *****    *** ****    ******  ******    *****    ***  *** ****    
  ***  *****  ***   *******   *** ******  *******   *********  ***  *** ******   *******   ***  *********   
  ***  ** **  ***  **** ****  ***  ***   ***   ***  ***   ***  ***       ***    ***   ***  ***  ***   ***   
  *** *** *** ***  ***   ***  ***  ***   *********  ***   ***  ******    ***    *********  ***  ***   ***   
   ****** ******   ***   ***  ***  ***   *********  ***   ***   ******   ***    *********  ***  ***   ***   
   *****   *****   ***   ***  ***  ***   ***        ***   ***    ******  ***    ***        ***  ***   ***   
   *****   *****   **** ****  ***  ***   ****  ***  ***   ***       ***  ***    ****  ***  ***  ***   ***   
   ****     ****    *******   ***  ***    *******   ***   ***  ***  ***  *****   *******   ***  ***   ***   
    ***     ***      *****    ***  ***     *****    ***   ***   ******    ****    *****    ***  ***   ***   
                                                                                                            
            ******** **                 ******                        *  **  **                             
            ******** **                 ******                       **  **  **                             
               **    **                 **                           **  **                                 
               **    ** **    ***       **      ** *  ****   ** **  **** **  **  ** **    ***               
               **    ******  *****      *****   ****  ****   ****** **** **  **  ******  *****              
               **    **  **  ** **      *****   **   **  **  **  **  **  **  **  **  **  ** **              
               **    **  **  *****      **      **   **  **  **  **  **  **  **  **  **  *****              
               **    **  **  **         **      **   **  **  **  **  **  **  **  **  **  **                 
               **    **  **  ** **      **      **   **  **  **  **  **  **  **  **  **  ** **              
               **    **  **  *****      **      **    ****   **  **  *** **  **  **  **  *****              
               **    **  **   ***       **      **    ****   **  **  *** **  **  **  **   ***               

*/


/* 

	Copyright (C) 2003-2005 Christopher Lais (aka "Zinx Verituse")
	and is covered by the following license:

	***
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. Modified source for this software, as used in any binaries you have
	distributed, must be provided on request, free of charge and/or penalty.

	4. This notice may not be removed or altered from any source distribution.
	***

*/
#include "q_shared.h"
#include "g_local.h"
#include "etpro_mdx.h"
#include "etpro_mdx_lut.h"


/* ******************* MDM/MDX file format, etc */

/* from http://games.theteamkillers.net/rtcw/mdx/ */
struct mdm_hdr
{
	char	ident[4];					/* "MDMW" */
	byte	version[4];					/* uint32 */
	char	filename[64];
	byte	lod_bias[4];				/* vec_t */
	byte	lod_scale[4];				/* vec_t */
	byte	surface_count[4];			/* uint32 */
	byte	surface_offset[4];			/* uint32 */
	byte	tag_count[4];				/* uint32 */
	byte	tag_offset[4];				/* uint32 */
	byte	eof_offset[4];				/* uint32 */
};
struct mdm_tag
{
	char	name[64];
	byte	axis[3][3][4];				/* vec_t[3][3] */
	byte	attach_bone[4];				/* uint32 */
	byte	offset[3][4];				/* vec_t[3] */
	byte	bone_count[4];				/* uint32 */
	byte	bone_offset[4];				/* uint32 */
	byte	tag_size[4];				/* uint32 */
	/* bone indexes (uint32) follow */
};
struct mdx_hdr
{
	char	ident[4];					/* "MDXW" */
	byte	version[4];					/* uint32 */
	char	filename[64];
	byte	frame_count[4];				/* uint32 */
	byte	bone_count[4];				/* uint32 */
	byte	frame_offset[4];			/* uint32 */
	byte	bone_offset[4];				/* uint32 */
	byte	torso_parent[4];			/* uint32 */
	byte	eof_offset[4];				/* uint32 */
};
struct mdx_frame_bone
{
	byte	angles[3][2];				/* int16[3] */
	byte	unused[2];					/* int16 */
	byte	offset_angles[2][2];		/* int16[2] */
};
struct mdx_frame
{
	byte	mins[3][4];					/* vec_t[3] */
	byte	maxs[3][4];					/* vec_t[3] */
	byte	origin[3][4];				/* vec_t[3] */
	byte	radius[4];					/* vec_t */
	byte	parent_offset[3][4];		/* vec_t[3] */
	/* mdx_frame_bones follow */
};
struct mdx_bone
{
	char	name[64];
	byte	parent_index[4];			/* int32 */
	byte	torso_weight[4];			/* vec_t */
	byte	parent_dist[4];				/* vec_t */
	byte	is_tag[4];					/* uint32 */
};
struct mdx
{
	struct mdx_hdr	*hdr;
	void			*frame;				/* struct mdx_frame; struct mdx_frame_bone*bone_count; ... */
	struct mdx_bone *bone;
	int				frames, bones;
};

/* ******************* Internal */
#ifdef BONE_HITTESTS
const char	*mdx_hit_type_names[MDX_HIT_TYPE_MAX] =
{
	"none",
	"gun",
	"head",
	"body",
	"arm_L",
	"arm_R",
	"leg_L",
	"leg_R",
};
#endif
struct bone
{
	char	name[64];
	int		parent_index;
	vec_t	parent_dist;
	vec_t	torso_weight;
};
struct frame_bone
{
	short	angles[3];					/* Orientation angle */
	short	offset_angles[2];			/* Offset angle */
};
struct frame
{
	vec_t				radius;
	vec3_t				parent_offset;
	struct frame_bone	*bones;
};
struct hit_area
{
	int						hit_type;
	animScriptImpactPoint_t impactpoint;
	int						tag[2];		/* internal (cached) tag numbers */
	vec3_t					scale[2];
	qboolean				ishead[2];
	qboolean				isbox;
	vec3_t					axis[3];	/* additional axis rotation before scale */
};
struct tag
{
	char	name[64];
	vec3_t	axis[3];
	vec3_t	offset;
	int		attach_bone;
};

/**************************************************************/
#define TAG_INTERNAL		( 1 << 30 )
#define TAG_INTERNAL_MASK	( ~TAG_INTERNAL )
#define INTERNTAG_TAG		( 1 << 29 ) /* based off tag, not bone */
#define INTERNTAG_TAG_MASK	( ~INTERNTAG_TAG )
typedef struct interntag_s	interntag_t;
typedef struct mdm_s		mdm_t;
typedef struct mdx_s		mdx_t;
typedef struct hit_s		hit_t;
struct interntag_s
{
	struct tag	tag;
	qboolean	merged;					/* merge with cachetag */
	float		weight;					/* weight for merge (1.0 = only first) */
	qboolean	ishead;					/* use head angles (for offset) */
};
struct mdm_s
{
	char		path[MAX_QPATH];
	int			tag_count;
	struct tag	*tags;

	/* quick lookup */
	int			tag_head, tag_footleft, tag_footright, tag_chest, tag_ubelt;
#ifdef BONE_HITTESTS
	int			*cachetags;				/* cachetag_count entries */
#endif /* BONE_HITTESTS */
};
struct mdx_s
{
	char			path[MAX_QPATH];
	int				bone_count;
	struct bone		*bones;
	int				frame_count;
	struct frame	*frames;
	int				torso_parent;
};
struct hit_s
{
	const animModelInfo_t	*animModelInfo;
	int						hit_count;
	struct hit_area			*hits;
};
static int			mdm_model_count = 0;
static mdm_t		*mdm_models = NULL;
static int			mdx_model_count = 0;
static mdx_t		*mdx_models = NULL;
#ifdef BONE_HITTESTS
static int			interntag_count = 0;
static interntag_t	*interntags = NULL;
static int			cachetag_count = 0;
static char ( *cachetag_names )[64] = NULL;
#endif /* BONE_HITTESTS */
static int		hit_count = 0;
static hit_t	*hits = NULL;

/* Space for calculated bone origins -- new calculations overwrite the previous */
static int		mdx_bones_max = 0;
static vec3_t	*mdx_bones = NULL;
#define INDEXTOQHANDLE( idx )	( qhandle_t ) ( (idx) + 1 )

// Index may be NULL sometimes, so just default to the first model (FIXME: This is a HACK.)
#define QHANDLETOINDEX( qh )			( (qh >= 1) ? ((int) (qh) - 1) : 0 )
#define QHANDLETOINDEX_SAFE( qh, old )	( (qh >= 1) ? (int) (qh) - 1 : QHANDLETOINDEX( old) )

/**************************************************************/


/* free allocated memory */
void
mdx_cleanup ( void )
{
	int i;
	mdx_bones_max = 0;
	free( mdx_bones );
	mdx_bones = NULL;
#ifdef BONE_HITTESTS
	cachetag_count = 0;
	free( cachetag_names );
	cachetag_names = NULL;
#endif /* BONE_HITTESTS */
	for ( i = 0; i < mdm_model_count; i++ )
	{
		free( mdm_models[i].tags );
#ifdef BONE_HITTESTS
		free( mdm_models[i].cachetags );
#endif /* BONE_HITTESTS */
	}

	mdm_model_count = 0;
	free( mdm_models );
	mdm_models = NULL;
	for ( i = 0; i < mdx_model_count; i++ )
	{
		free( mdx_models[i].bones );
		free( mdx_models[i].frames );
	}

	mdx_model_count = 0;
	free( mdx_models );
	mdx_models = NULL;
	for ( i = 0; i < hit_count; i++ )
	{
		free( hits[i].hits );
	}

	hit_count = 0;
	free( hits );
	hits = NULL;
}

/**************************************************************/

/* Utility functions */


/* VectorRotate uses the transpose; I have no idea what use it is. */
static void
PointRotate ( const vec3_t in, vec3_t axis[3], vec3_t out )
{
	out[0] = in[0] * axis[0][0] + in[1] * axis[1][0] + in[2] * axis[2][0];
	out[1] = in[0] * axis[0][1] + in[1] * axis[1][1] + in[2] * axis[2][1];
	out[2] = in[0] * axis[0][2] + in[1] * axis[1][2] + in[2] * axis[2][2];
}


/* */
static void
MatrixTranspose ( /*const*/ vec3_t in[3], vec3_t out[3] )
{
	int i, j;
	for ( i = 0; i < 3; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			out[i][j] = in[j][i];
		}
	}
}


/* */
static void
MatrixWeight ( /*const*/ vec3_t m[3], float weight, vec3_t mout[3] )
{
	float	one = 1.0 - weight;
	mout[0][0] = m[0][0] * weight + one;
	mout[0][1] = m[0][1] * weight;
	mout[0][2] = m[0][2] * weight;
	mout[1][0] = m[1][0] * weight;
	mout[1][1] = m[1][1] * weight + one;
	mout[1][2] = m[1][2] * weight;
	mout[2][0] = m[2][0] * weight;
	mout[2][1] = m[2][1] * weight;
	mout[2][2] = m[2][2] * weight + one;
}


/* The engine transforms short angles to an axis somewhat brokenly -
   it uses a LUT and has truely perplexing values */
static void
AnglesToAxisBroken ( const short angles[2], vec3_t matrix[3] )
{
	int		idx;
	float	sp, sy, cp, cy;
	idx = angles[0] >> 4;
	if ( idx < 0 )
	{
		idx += 4096;
	}

	sp = sintable[idx];
	cp = sintable[( idx + 1024 ) % 4096];
	idx = angles[1] >> 4;
	if ( idx < 0 )
	{
		idx += 4096;
	}

	sy = sintable[idx];
	cy = sintable[( idx + 1024 ) % 4096];
	matrix[0][0] = cp * cy;
	matrix[0][1] = cp * sy;
	matrix[0][2] = -sp;
	matrix[1][0] = -sy;
	matrix[1][1] = cy;
	matrix[1][2] = 0;
	matrix[2][0] = sp * cy;
	matrix[2][1] = sp * sy;
	matrix[2][2] = cp;
}

#ifdef BONE_HITTESTS


/* */
static void
mdx_matrix_to_quaternion ( vec3_t m[3], vec4_t q )
{
	vec_t	w4;
	q[3] = sqrt( 1.0 + m[0][0] + m[1][1] + m[2][2] ) / 2.0;
	w4 = q[3] * 4.0;
	q[0] = ( m[1][2] - m[2][1] ) / w4;
	q[1] = ( m[2][0] - m[0][2] ) / w4;
	q[2] = ( m[0][1] - m[1][0] ) / w4;
}


/* */
static void
mdx_quaternion_to_matrix ( vec4_t q, vec3_t m[3] )
{
	m[0][0] = 1 - 2 * ( q[1] * q[1] + q[2] * q[2] );
	m[0][1] = 2 * ( q[0] * q[1] + q[2] * q[3] );
	m[0][2] = 2 * ( q[0] * q[2] - q[1] * q[3] );
	m[1][0] = 2 * ( q[0] * q[1] - q[2] * q[3] );
	m[1][1] = 1 - 2 * ( q[0] * q[0] + q[2] * q[2] );
	m[1][2] = 2 * ( q[1] * q[2] + q[0] * q[3] );
	m[2][0] = 2 * ( q[0] * q[2] + q[1] * q[3] );
	m[2][1] = 2 * ( q[1] * q[2] - q[0] * q[3] );
	m[2][2] = 1 - 2 * ( q[0] * q[0] + q[1] * q[1] );
}


/* */
static void
mdx_quaternion_nlerp ( const vec4_t q1, const vec4_t q2, vec4_t qout, float backlerp )
{
	float	fwdlerp = 1.0 - backlerp;
	float	len;
	qout[0] = q1[0] * backlerp + q2[0] * fwdlerp;
	qout[1] = q1[1] * backlerp + q2[1] * fwdlerp;
	qout[2] = q1[2] * backlerp + q2[2] * fwdlerp;
	qout[3] = q1[3] * backlerp + q2[3] * fwdlerp;
	len = sqrt( qout[0] * qout[0] + qout[1] * qout[1] + qout[2] * qout[2] + qout[3] * qout[3] );
	if ( len )
	{
		qout[0] /= len;
		qout[1] /= len;
		qout[2] /= len;
		qout[3] /= len;
	}
	else
	{

		// very rare -- quaternions pointing in opposite direction with backlerp 0.5
		// since they're exactly opposite, they're the same rotation :x
		Vector4Copy( q1, qout );
	}
}


// lerp two rotation matrcies; too lazy to work out how to do it in matrix space.
static void
mdx_lerp_matrix ( vec3_t m1[3], vec3_t m2[3], vec3_t mout[3], float backlerp )
{
	vec4_t	q1, q2, q;
	mdx_matrix_to_quaternion( m1, q1 );
	mdx_matrix_to_quaternion( m2, q2 );
	mdx_quaternion_nlerp( q1, q2, q, backlerp );
	mdx_quaternion_to_matrix( q, mout );
}
#endif /* BONE_HITTESTS */

/* */
void
mdx_gentity_to_grefEntity ( gentity_t *ent, grefEntity_t *refent, int lerpTime )
{
	bg_character_t	*character;
	vec3_t			legsAngles, torsoAngles, headAngles;
	memset( refent, 0, sizeof(*refent) );
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	refent->hModel = character->mesh;
	VectorCopy( ent->r.currentOrigin, refent->origin );
	refent->frame = ent->legsFrame.frame;
	refent->frameModel = ent->legsFrame.frameModel;
	refent->oldframe = ent->legsFrame.oldFrame;
	refent->oldframeModel = ent->legsFrame.oldFrameModel;
	if ( ent->legsFrame.frameTime == ent->legsFrame.oldFrameTime )
	{
		refent->backlerp = 0.0;
	}
	else
	{
		refent->backlerp = 1.0 - (float) ( lerpTime - ent->legsFrame.oldFrameTime ) / ( ent->legsFrame.frameTime - ent->legsFrame.oldFrameTime );
	}

	refent->torsoFrame = ent->torsoFrame.frame;
	refent->torsoFrameModel = ent->torsoFrame.frameModel;
	refent->oldTorsoFrame = ent->torsoFrame.oldFrame;
	refent->oldTorsoFrameModel = ent->torsoFrame.oldFrameModel;
	if ( ent->torsoFrame.frameTime == ent->torsoFrame.oldFrameTime )
	{
		refent->torsoBacklerp = 0.0;
	}
	else
	{
		refent->torsoBacklerp = 1.0 - (float) ( lerpTime - ent->torsoFrame.oldFrameTime ) / ( ent->torsoFrame.frameTime - ent->torsoFrame.oldFrameTime );
	}

	mdx_PlayerAngles( ent, legsAngles, torsoAngles, headAngles, qfalse );
	AnglesToAxis( legsAngles, refent->axis );
	AnglesToAxis( torsoAngles, refent->torsoAxis );
	AnglesToAxis( headAngles, refent->headAxis );
}

/**************************************************************/

/* tag management */
#ifdef BONE_HITTESTS
static int	mdm_tag_lookup ( const mdm_t *model, const char tagName[64] );


/* */
static interntag_t *
interntag_alloc ( void )
{
	interntag_t *tag;
	interntags = realloc( interntags, (interntag_count + 1) * sizeof(*interntags) );
	tag = &interntags[interntag_count++];
	tag->tag.name[0] = '\0';
	AxisCopy( axisDefault, tag->tag.axis );
	VectorClear( tag->tag.offset );
	tag->merged = -1;
	tag->weight = 0.5;
	tag->ishead = qfalse;
	return ( tag );
}


/* */
static void
interntag_dealloc ( void )
{
	--interntag_count;
}


/* */
static int
cachetag_cache ( const char tagName[64] )
{
	int i;
	for ( i = 0; i < cachetag_count; i++ )
	{
		if ( !Q_stricmp( cachetag_names[i], tagName) )
		{
			return ( i );
		}
	}

	cachetag_names = realloc( cachetag_names, (cachetag_count + 1) * sizeof(*cachetag_names) );
	Q_strncpyz( &cachetag_names[cachetag_count][0], tagName, sizeof(*cachetag_names) );
	return ( cachetag_count++ );
}


/* */
static void
mdm_cachetag_resize ( mdm_t *model, int oldcount )
{
	int i;
	model->cachetags = realloc( model->cachetags, cachetag_count * sizeof(*model->cachetags) );
	for ( i = oldcount; i < cachetag_count; i++ )
	{
		model->cachetags[i] = mdm_tag_lookup( model, cachetag_names[i] );
		if ( model->cachetags[i] == -1 )
		{
			G_Printf( S_COLOR_YELLOW GAME_VERSION " MDX WARNING: Unable to find tag %s in model %s\n",
					  cachetag_names[i], model->path );
			model->cachetags[i] = 0;	// FIXME: Do proper handling of tags that aren't in models..
		}
	}
}


/* */
static void
cachetag_resize ( int oldcount )
{
	int i;
	for ( i = 0; i < mdm_model_count; i++ )
	{
		mdm_cachetag_resize( &mdm_models[i], oldcount );
	}
}
#endif /* BONE_HITTESTS */

/**************************************************************/

/* File I/O */
static int
mdx_read_int ( const byte *data )
{
	return ( data[0] << 0 ) | ( data[1] << 8 ) | ( data[2] << 16 ) | ( data[3] << 24 );
}


/* */
static short
mdx_read_short ( const byte *data )
{
	return ( data[0] << 0 ) | ( data[1] << 8 );
}


/* */
static vec_t
mdx_read_vec ( const byte *data )
{

	/* FIXME: depends on size of int */
	int int_val = ( data[0] << 0 ) | ( data[1] << 8 ) | ( data[2] << 16 ) | ( data[3] << 24 );
	return ( *(float *) &int_val );
}

#ifdef BONE_HITTESTS


/* */
static int
mdx_bone_lookup ( const mdx_t *mdxModel, const char *name )
{
	int i;
	for ( i = 0; i < mdxModel->bone_count; i++ )
	{
		if ( !strcmp( mdxModel->bones[i].name, name) )
		{
			return ( i );
		}
	}

	return ( -1 );
}
#endif /* BONE_HITTESTS */

/* */
static int
mdm_tag_lookup ( const mdm_t *model, const char tagName[64] )
{
	int i;
	for ( i = 0; i < model->tag_count; i++ )
	{
		if ( !Q_stricmp( model->tags[i].name, tagName) )
		{
			return ( i );
		}
	}

#ifdef BONE_HITTESTS
	for ( i = 0; i < interntag_count; i++ )
	{
		if ( !Q_stricmp( interntags[i].tag.name, tagName) )
		{
			return ( i | TAG_INTERNAL );
		}
	}
#endif /* BONE_HITTESTS */
	return ( -1 );
}


/* */
static void
mdx_load ( mdx_t *mdxModel, char *mem )
{
	char			*ptr;
	int				frame_count, bone_count;
	int				frame_offset, bone_offset;
	struct mdx_hdr	*hdr;
	struct mdx_bone *bones;
	char			*frames;
	int				i, j;
	hdr = (void *) mem;
	frame_offset = mdx_read_int( hdr->frame_offset );
	frames = (void *) ( mem + frame_offset );
	frame_count = mdx_read_int( hdr->frame_count );
	bone_offset = mdx_read_int( hdr->bone_offset );
	bones = (void *) ( mem + bone_offset );
	bone_count = mdx_read_int( hdr->bone_count );
	mdxModel->torso_parent = mdx_read_int( hdr->torso_parent );
	if ( bone_count > mdx_bones_max )
	{
		free( mdx_bones );
		mdx_bones_max = bone_count;
		mdx_bones = malloc( mdx_bones_max * sizeof(*mdx_bones) );
	}

	/* Load bones */
	mdxModel->bone_count = bone_count;
	free( mdxModel->bones );
	mdxModel->bones = malloc( mdxModel->bone_count * sizeof(struct bone) );
	for ( i = 0; i < mdxModel->bone_count; i++ )
	{
		struct bone *bone = &mdxModel->bones[i];
		bone->parent_index = mdx_read_int( bones[i].parent_index );
		if ( bone->parent_index >= i )
		{
			G_Error( GAME_VERSION " MDX: parent_index >= index\n" );
		}

		Q_strncpyz( bone->name, bones[i].name, sizeof(bone->name) );
		bone->parent_dist = mdx_read_vec( bones[i].parent_dist );
		bone->torso_weight = mdx_read_vec( bones[i].torso_weight );
	}

	/* Load frames */
	mdxModel->frame_count = frame_count;
	free( mdxModel->frames );
	ptr = malloc( mdxModel->frame_count * (sizeof(struct frame) + mdxModel->bone_count * sizeof(struct frame_bone)) );
	mdxModel->frames = (void *) ptr;
	ptr += mdxModel->frame_count * sizeof( struct frame );
	for ( i = 0; i < mdxModel->frame_count; i++ )
	{
		struct mdx_frame		*frame = (void *)
			( frames + i * (sizeof(struct mdx_frame) + sizeof(struct mdx_frame_bone) * bone_count) );
		struct mdx_frame_bone	*frame_bone = (void *) &frame[1];
		struct frame_bone		*bones;
		bones = mdxModel->frames[i].bones = (void *) ptr;
		ptr += mdxModel->bone_count * sizeof( struct frame_bone );
		mdxModel->frames[i].radius = mdx_read_vec( frame->radius );
		mdxModel->frames[i].parent_offset[0] = mdx_read_vec( frame->parent_offset[0] );
		mdxModel->frames[i].parent_offset[1] = mdx_read_vec( frame->parent_offset[1] );
		mdxModel->frames[i].parent_offset[2] = mdx_read_vec( frame->parent_offset[2] );
		for ( j = 0; j < mdxModel->bone_count; j++ )
		{
			bones[j].angles[0] = mdx_read_short( frame_bone[j].angles[0] );
			bones[j].angles[1] = mdx_read_short( frame_bone[j].angles[1] );
			bones[j].angles[2] = mdx_read_short( frame_bone[j].angles[2] );
			bones[j].offset_angles[0] = mdx_read_short( frame_bone[j].offset_angles[0] );
			bones[j].offset_angles[1] = mdx_read_short( frame_bone[j].offset_angles[1] );
		}
	}
}


/* */
static void
mdm_load ( mdm_t *mdmModel, char *mem )
{
	struct mdm_hdr	*hdr;
	struct mdm_tag	*tag;
	int				i, tags;
	hdr = (void *) mem;
	tags = mdx_read_int( hdr->tag_count );
	tag = (void *) ( mem + mdx_read_int( hdr->tag_offset) );
	free( mdmModel->tags );
	mdmModel->tag_count = tags;
	mdmModel->tags = malloc( mdmModel->tag_count * sizeof(struct tag) );
	mdmModel->tag_head = mdmModel->tag_footleft = mdmModel->tag_footright = mdmModel->tag_ubelt = mdmModel->tag_chest = -1;
#ifdef BONE_HITTESTS
	mdmModel->cachetags = NULL;
#endif /* BONE_HITTESTS */
	for ( i = 0; i < tags; i++ )
	{
		int n, p;
		Q_strncpyz( mdmModel->tags[i].name, tag->name, sizeof(mdmModel->tags[i].name) );

		// lookup
		if ( !Q_stricmp( tag->name, "tag_head") )
		{
			mdmModel->tag_head = i;
		}
		else if ( !Q_stricmp( tag->name, "tag_footleft") )
		{
			mdmModel->tag_footleft = i;
		}
		else if ( !Q_stricmp( tag->name, "tag_footright") )
		{
			mdmModel->tag_footright = i;
		}
		else if ( !Q_stricmp( tag->name, "tag_chest") )
		{
			mdmModel->tag_chest = i;
		}
		else if ( !Q_stricmp( tag->name, "tag_ubelt") )
		{
			mdmModel->tag_ubelt = i;
		}

		for ( n = 0; n < 3; n++ )
		{
			for ( p = 0; p < 3; p++ )
			{
				mdmModel->tags[i].axis[n][p] = mdx_read_vec( tag->axis[n][p] );
			}
		}

		for ( p = 0; p < 3; p++ )
		{
			mdmModel->tags[i].offset[p] = mdx_read_vec( tag->offset[p] );
		}

		mdmModel->tags[i].attach_bone = mdx_read_int( tag->attach_bone );
		tag = (void *) ( ((char *) tag) + mdx_read_int( tag->tag_size) );
	}

#ifdef BONE_HITTESTS
	mdm_cachetag_resize( mdmModel, 0 );
#endif /* BONE_HITTESTS */
}

#ifdef BONE_HITTESTS


/* */
static qboolean
hit_parse_tag ( hit_t *hitModel, mdx_t *mdx, char **ptr )
{
	char		*token;
	interntag_t *firsttag, *tag;
	int			bone;
	firsttag = tag = interntag_alloc();
	token = COM_ParseExt( ptr, qfalse );
	if ( !token[0] )
	{
		COM_ParseError( "Expected new bone/tag name\n" );
		goto err;
	}

	Q_strncpyz( tag->tag.name, token, sizeof(tag->tag.name) );
	token = COM_ParseExt( ptr, qfalse );
	if ( !token[0] )
	{
		COM_ParseError( "Expected bone/tag name\n" );
		goto err;
	}

	bone = mdx_bone_lookup( mdx, token );
	if ( bone == -1 )
	{
		bone = cachetag_cache( token ) | INTERNTAG_TAG;
	}

	tag->tag.attach_bone = bone;
	while ( ptr )
	{
		token = COM_ParseExt( ptr, qfalse );
		if ( !token[0] )
		{
			break;
		}

		if ( !Q_stricmp( token, "weight") )
		{
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected weight\n" );
				goto err;
			}

			firsttag->weight = strtod( token, NULL );
		}
		else if ( !Q_stricmp( token, "offset") )
		{
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected X offset\n" );
				goto err;
			}

			tag->tag.offset[0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected Y offset\n" );
				goto err;
			}

			tag->tag.offset[1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected Z offset\n" );
				goto err;
			}

			tag->tag.offset[2] = strtod( token, NULL );
		}
		else if ( !Q_stricmp( token, "axis") )
		{
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected forward vector[X]\n" );
				goto err;
			}

			tag->tag.axis[0][0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected forward vector[Y]\n" );
				goto err;
			}

			tag->tag.axis[0][1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected forward vector[Z]\n" );
				goto err;
			}

			tag->tag.axis[0][2] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected left vector[X]\n" );
				goto err;
			}

			tag->tag.axis[1][0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected left vector[Y]\n" );
				goto err;
			}

			tag->tag.axis[1][1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected left vector[Z]\n" );
				goto err;
			}

			tag->tag.axis[1][2] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected up vector[X]\n" );
				goto err;
			}

			tag->tag.axis[2][0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected up vector[Y]\n" );
				goto err;
			}

			tag->tag.axis[2][1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected up vector[Z]\n" );
				goto err;
			}

			tag->tag.axis[2][2] = strtod( token, NULL );
		}
		else if ( !Q_stricmp( token, "headangles") )
		{
			tag->ishead = qtrue;
		}
		else
		{
			if ( tag != firsttag )
			{
				COM_ParseError( "Too many bones/tags for internal tag\n" );
				goto err;
			}

			bone = mdx_bone_lookup( mdx, token );
			if ( bone == -1 )
			{
				bone = cachetag_cache( token ) | INTERNTAG_TAG;
			}

			tag = interntag_alloc();
			firsttag = tag - 1; /* interntag_alloc reallocs :x */
			tag->tag.attach_bone = bone;
			Q_strncpyz( tag->tag.name, va( "_%s_m", firsttag->tag.name), sizeof(tag->tag.name) );
			firsttag->merged = cachetag_cache( tag->tag.name );
		}
	}

	return ( qtrue );
err:
	if ( firsttag != tag )
	{
		interntag_dealloc();
	}

	interntag_dealloc();
	return ( qfalse );
}


/* */
static qboolean
hit_parse_hit ( hit_t *hitModel, mdx_t *mdx, char **ptr )
{
	char			*token;
	struct hit_area *hit;
	int				tagidx;
	int				tag;
	hitModel->hits = realloc( hitModel->hits, (hitModel->hit_count + 1) * sizeof(*hitModel->hits) );
	hit = &hitModel->hits[hitModel->hit_count++];
	for ( tagidx = 0; tagidx < 2; tagidx++ )
	{
		hit->hit_type = -1;
		hit->impactpoint = NUM_ANIM_COND_IMPACTPOINT;
		hit->tag[tagidx] = -1;
		VectorSet( hit->scale[tagidx], 1.0, 1.0, 1.0 );
		AxisCopy( axisDefault, hit->axis );
		hit->ishead[tagidx] = qfalse;
	}

	token = COM_ParseExt( ptr, qfalse );
	if ( !token[0] )
	{
		COM_ParseError( "Expected hit type\n" );
		goto err;
	}

	for ( hit->hit_type = 0; hit->hit_type < MDX_HIT_TYPE_MAX; hit->hit_type++ )
	{
		if ( !Q_stricmp( mdx_hit_type_names[hit->hit_type], token) )
		{
			break;
		}
	}

	if ( hit->hit_type >= MDX_HIT_TYPE_MAX )
	{
		COM_ParseError( "Invalid hit type: %s\n", token );
		goto err;
	}

	tagidx = -1;
	while ( ptr )
	{
		token = COM_ParseExt( ptr, qfalse );
		if ( !token[0] )
		{
			break;
		}

		if ( tagidx > -1 )
		{
			if ( !Q_stricmp( token, "scale") )
			{
				token = COM_ParseExt( ptr, qfalse );
				if ( !token[0] )
				{
					COM_ParseError( "Expected X scale\n" );
					goto err;
				}

				hit->scale[tagidx][0] *= strtod( token, NULL );
				token = COM_ParseExt( ptr, qfalse );
				if ( !token[0] )
				{
					COM_ParseError( "Expected Y scale\n" );
					goto err;
				}

				hit->scale[tagidx][1] *= strtod( token, NULL );
				token = COM_ParseExt( ptr, qfalse );
				if ( !token[0] )
				{
					COM_ParseError( "Expected Z scale\n" );
					goto err;
				}

				hit->scale[tagidx][2] *= strtod( token, NULL );
				continue;
			}
			else if ( !Q_stricmp( token, "radius") )
			{
				float	radius;
				token = COM_ParseExt( ptr, qfalse );
				if ( !token[0] )
				{
					COM_ParseError( "Expected radius\n" );
					goto err;
				}

				radius = strtod( token, NULL );
				hit->scale[tagidx][0] *= radius;
				hit->scale[tagidx][1] *= radius;
				hit->scale[tagidx][2] *= radius;
				continue;
			}
			else if ( !Q_stricmp( token, "headangles") )
			{
				hit->ishead[tagidx] = qtrue;
				continue;
			}
		}

		if ( !Q_stricmp( token, "axis") )
		{
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected forward vector [X]\n" );
				goto err;
			}

			hit->axis[0][0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected forward vector [Y]\n" );
				goto err;
			}

			hit->axis[0][1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected forward vector [Z]\n" );
				goto err;
			}

			hit->axis[0][2] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected left vector [X]\n" );
				goto err;
			}

			hit->axis[1][0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected left vector [Y]\n" );
				goto err;
			}

			hit->axis[1][1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected left vector [Z]\n" );
				goto err;
			}

			hit->axis[1][2] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected up vector [X]\n" );
				goto err;
			}

			hit->axis[2][0] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected up vector [Y]\n" );
				goto err;
			}

			hit->axis[2][1] = strtod( token, NULL );
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected up vector [Z]\n" );
				goto err;
			}

			hit->axis[2][2] = strtod( token, NULL );
			continue;
		}
		else if ( !Q_stricmp( token, "impactpoint") || !Q_stricmp( token, "impact") )
		{
			token = COM_ParseExt( ptr, qfalse );
			if ( !token[0] )
			{
				COM_ParseError( "Expected impact point name/number\n" );
				goto err;
			}

			if ( !Q_stricmp( token, "head") )
			{
				hit->impactpoint = IMPACTPOINT_HEAD;
			}
			else if ( !Q_stricmp( token, "chest") )
			{
				hit->impactpoint = IMPACTPOINT_CHEST;
			}
			else if ( !Q_stricmp( token, "gut") )
			{
				hit->impactpoint = IMPACTPOINT_GUT;
			}
			else if ( !Q_stricmp( token, "groin") )
			{
				hit->impactpoint = IMPACTPOINT_GROIN;
			}
			else if ( !Q_stricmp( token, "shoulder_right") )
			{
				hit->impactpoint = IMPACTPOINT_SHOULDER_RIGHT;
			}
			else if ( !Q_stricmp( token, "shoulder_left") )
			{
				hit->impactpoint = IMPACTPOINT_SHOULDER_LEFT;
			}
			else if ( !Q_stricmp( token, "knee_right") )
			{
				hit->impactpoint = IMPACTPOINT_KNEE_RIGHT;
			}
			else if ( !Q_stricmp( token, "knee_left") )
			{
				hit->impactpoint = IMPACTPOINT_KNEE_LEFT;
			}
			else
			{
				hit->impactpoint = atoi( token );
				if ( hit->impactpoint < 0 || hit->impactpoint > NUM_ANIM_COND_IMPACTPOINT )
				{
					hit->impactpoint = NUM_ANIM_COND_IMPACTPOINT;
				}
			}

			continue;
		}
		else if ( !Q_stricmp( token, "box") )
		{
			hit->isbox = qtrue;
			continue;
		}

		tag = cachetag_cache( token );
		if ( tag == -1 )
		{
			COM_ParseError( "Unexpected token: %s\n", token );
			goto err;
		}

		if ( ++tagidx >= 2 )
		{
			COM_ParseError( "Too many tags for hit\n" );
			goto err;
		}

		hit->tag[tagidx] = tag;
	}

	if ( tagidx == -1 )
	{
		COM_ParseError( "Expected bone/tag name\n" );
		goto err;
	}

	return ( qtrue );
err:
	hitModel->hit_count--;
	return ( qfalse );
}

/* Must be called _AFTER_ animModelInfo has valid animations[0] */


/* Assumes all animations have mdxFiles with the same bones, which
   I *hope* is a pretty safe assumption. */
static qboolean
hit_load ( hit_t *hitModel, const animModelInfo_t *animModelInfo, const char *filename )
{
	mdx_t			*mdx;
	char			*ptr, *token;
	char			*pScript;
	int				len;
	fileHandle_t	fh;
	int				cachetag_oldcount;
	cachetag_oldcount = cachetag_count;
	mdx = &mdx_models[animModelInfo->animations[0]->mdxFile];
	hitModel->animModelInfo = animModelInfo;
	len = trap_FS_FOpenFile( filename, &fh, FS_READ );
	if ( len <= 0 )
	{

		/* zinx - don't show this normally, for now..  , jaquboss, show it! */
		G_Printf( S_COLOR_YELLOW GAME_VERSION " MDX WARNING: Missing %s (only needed for per-bone hits)\n", filename );
		return ( qfalse );
	}

	ptr = pScript = malloc( len + 1 );
	trap_FS_Read( pScript, len, fh );
	pScript[len] = '\0';
	trap_FS_FCloseFile( fh );
	free( hitModel->hits );
	hitModel->hits = NULL;
	COM_SetCurrentParseLine( 1 );
	while ( ptr )
	{
		token = COM_Parse( &ptr );
		if ( !token[0] )
		{
			continue;
		}

		if ( !Q_stricmp( token, "TAG") )
		{
			if ( !hit_parse_tag( hitModel, mdx, &ptr) )
			{
				goto err;
			}

			continue;
		}

		if ( !Q_stricmp( token, "HIT") )
		{
			if ( !hit_parse_hit( hitModel, mdx, &ptr) )
			{
				goto err;
			}

			continue;
		}

		COM_ParseError( "Unexpected token: %s\n", token );
		goto err;
	}

	cachetag_resize( cachetag_oldcount );
	free( pScript );
	return ( qtrue );
err:
	cachetag_resize( cachetag_oldcount );
	free( pScript );
	free( hitModel->hits );
	hitModel->hit_count = 0;
	hitModel->hits = NULL;
	return ( qfalse );
}
#endif /* BONE_HITTESTS */

/* */
qhandle_t
trap_R_RegisterModel ( const char *filename )
{
	fileHandle_t	fh;
	char			*mem;
	int				ret;
	int				i;
	int				len;
	for ( i = 0; i < mdm_model_count; i++ )
	{
		if ( !strcmp( mdm_models[i].path, filename) )
		{
			return ( INDEXTOQHANDLE( i) );
		}
	}

	for ( i = 0; i < mdx_model_count; i++ )
	{
		if ( !strcmp( mdx_models[i].path, filename) )
		{
			return ( INDEXTOQHANDLE( i) );
		}
	}

	len = trap_FS_FOpenFile( filename, &fh, FS_READ );
	if ( len <= 0 )
	{
		G_Error( GAME_VERSION " MDX: File not found: %s\n", filename );
	}

	mem = malloc( len );
	trap_FS_Read( mem, len, fh );
	trap_FS_FCloseFile( fh );
	if ( !memcmp( mem, "MDXW", 4) )
	{
		ret = mdx_model_count++;
		mdx_models = realloc( mdx_models, mdx_model_count * sizeof(*mdx_models) );
		memset( &mdx_models[ret], 0, sizeof(mdx_models[ret]) );
		Q_strncpyz( mdx_models[ret].path, filename, sizeof(mdx_models[ret].path) );
		mdx_load( &mdx_models[ret], mem );
	}
	else if ( !memcmp( mem, "MDMW", 4) )
	{
		ret = mdm_model_count++;
		mdm_models = realloc( mdm_models, mdm_model_count * sizeof(*mdm_models) );
		memset( &mdm_models[ret], 0, sizeof(mdm_models[ret]) );
		Q_strncpyz( mdm_models[ret].path, filename, sizeof(mdm_models[ret].path) );
		mdm_load( &mdm_models[ret], mem );
	}
	else
	{
		ret = -1;
		G_Error( GAME_VERSION " MDX: Not a model: %s\n", filename );
	}

	free( mem );
	return ( INDEXTOQHANDLE( ret) );
}

#ifdef BONE_HITTESTS


/* */
qhandle_t
mdx_RegisterHits ( animModelInfo_t *animModelInfo, const char *filename )
{
	int i;
	for ( i = 0; i < hit_count; i++ )
	{
		if ( hits[i].animModelInfo == animModelInfo )
		{
			return ( i );
		}
	}

	i = hit_count++;
	hits = realloc( hits, hit_count * sizeof(*hits) );
	memset( &hits[i], 0, sizeof(hits[i]) );
	if ( hit_load( &hits[i], animModelInfo, filename) < 0 )
	{
		hit_count--;
		return ( 0 );
	}
	else
	{
		return ( INDEXTOQHANDLE( i) );
	}
}
#endif /* BONE_HITTESTS */

/**************************************************************/

/* Bone Calculations */
static void
mdx_calculate_bone ( vec3_t dest, const struct bone *bone, const struct frame_bone *frameBone )
{
	vec3_t	tmp;
	vec3_t	axis[3];
	tmp[1] = tmp[2] = 0;
	tmp[0] = bone->parent_dist;

	/* frame bone rotation */
	AnglesToAxisBroken( frameBone->offset_angles, axis );
	PointRotate( tmp, axis, dest );
}


/* */
static void
mdx_calculate_bone_lerp
(
	/*const*/grefEntity_t	*refent,
	mdx_t					*frameModel,
	mdx_t					*oldFrameModel,
	mdx_t					*torsoFrameModel,
	mdx_t					*oldTorsoFrameModel,
	int						i,
	qboolean				recursive
)
{
	mdx_t				*oldBoneFrameModel, *boneFrameModel;
	int					oldFrame, frame;
	float				backlerp;
	struct bone			*oldBone, *bone;
	struct frame_bone	*oldFrameBone, *frameBone;
	vec3_t				point, oldpoint;
	if ( frameModel->bones[i].torso_weight )
	{
		boneFrameModel = torsoFrameModel;
		oldBoneFrameModel = oldTorsoFrameModel;
		frame = refent->torsoFrame;
		oldFrame = refent->oldTorsoFrame;
		backlerp = refent->torsoBacklerp;
	}
	else
	{
		boneFrameModel = frameModel;
		oldBoneFrameModel = oldFrameModel;
		frame = refent->frame;
		oldFrame = refent->oldframe;
		backlerp = refent->backlerp;
	}

	bone = &boneFrameModel->bones[i];
	oldBone = &oldBoneFrameModel->bones[i];
	if ( i == 0 )
	{
		VectorMA( vec3_origin, 1.0 - backlerp, boneFrameModel->frames[frame].parent_offset, mdx_bones[i] );
		VectorMA( mdx_bones[i], backlerp, oldBoneFrameModel->frames[oldFrame].parent_offset, mdx_bones[i] );
		return; // It's offset funny if we do the calculations for the top-most bone
	}
	else
	{
		if ( recursive )
		{
			mdx_calculate_bone_lerp( refent, frameModel, oldFrameModel, torsoFrameModel, oldTorsoFrameModel,
									 bone->parent_index, qtrue );
		}
	}

	if (frame < 0)
		frame = 0;

	frameBone = &boneFrameModel->frames[frame].bones[i];

	if (oldFrame < 0)
		oldFrame = 0;

	oldFrameBone = &oldBoneFrameModel->frames[oldFrame].bones[i];

	if (!oldFrameBone)
		return;

	mdx_calculate_bone( oldpoint, oldBone, oldFrameBone );
	mdx_calculate_bone( point, bone, frameBone );

	/* This frame's position */
	VectorAdd( mdx_bones[bone->parent_index], point, mdx_bones[i] );

	/* Lerp in old frame */
	VectorSubtract( oldpoint, point, oldpoint );
	VectorMA( mdx_bones[i], backlerp, oldpoint, mdx_bones[i] );
}

#ifdef BONE_HITTESTS


/* Calculates all bones */
static void
mdx_calculate_bones ( /*const*/ grefEntity_t *refent )
{
	int		i;
	mdx_t	*frameModel = &mdx_models[QHANDLETOINDEX( refent->frameModel )];
	mdx_t	*oldFrameModel = &mdx_models[QHANDLETOINDEX_SAFE( refent->oldframeModel, refent->frameModel )];
	mdx_t	*torsoFrameModel = &mdx_models[QHANDLETOINDEX( refent->torsoFrameModel )];
	mdx_t	*oldTorsoFrameModel = &mdx_models[QHANDLETOINDEX_SAFE( refent->oldTorsoFrameModel, refent->torsoFrameModel )];
#ifdef DEBUG
	if
	(
		frameModel->bone_count != torsoFrameModel->bone_count ||
		frameModel->bone_count != oldFrameModel->bone_count ||
		frameModel->bone_count != oldTorsoFrameModel->bone_count
	)
	{
		G_Error( GAME_VERSION " MDX: Frame count mismatch\n" );
	}
#endif
	for ( i = 0; i < frameModel->bone_count; i++ )
	{
		mdx_calculate_bone_lerp( refent, frameModel, oldFrameModel, torsoFrameModel, oldTorsoFrameModel, i, qfalse );
	}
}
#endif /* BONE_HITTESTS */

/* */
void
mdx_calculate_bones_single ( /*const*/ grefEntity_t *refent, int i )
{
	mdx_t	*frameModel = &mdx_models[QHANDLETOINDEX( refent->frameModel )];
	mdx_t	*oldFrameModel = &mdx_models[QHANDLETOINDEX_SAFE( refent->oldframeModel, refent->frameModel )];
	mdx_t	*torsoFrameModel = &mdx_models[QHANDLETOINDEX( refent->torsoFrameModel )];
	mdx_t	*oldTorsoFrameModel = &mdx_models[QHANDLETOINDEX_SAFE( refent->oldTorsoFrameModel, refent->torsoFrameModel )];
#ifdef DEBUG
	if
	(
		frameModel->bone_count != torsoFrameModel->bone_count ||
		frameModel->bone_count != oldFrameModel->bone_count ||
		frameModel->bone_count != oldTorsoFrameModel->bone_count
	)
	{
		G_Error( GAME_VERSION " MDX: Frame count mismatch\n" );
	}
#endif
	mdx_calculate_bone_lerp( refent, frameModel, oldFrameModel, torsoFrameModel, oldTorsoFrameModel, i, qtrue );
}


/* */
static void
mdx_bone_orientation ( /*const*/ grefEntity_t *refent, int idx, vec3_t origin, vec3_t axis[3] )
{
	mdx_t				*frameModel = &mdx_models[QHANDLETOINDEX( refent->frameModel )];
	mdx_t				*oldFrameModel = &mdx_models[QHANDLETOINDEX_SAFE( refent->oldframeModel, refent->frameModel )];
	mdx_t				*torsoFrameModel = &mdx_models[QHANDLETOINDEX( refent->torsoFrameModel )];
	mdx_t				*oldTorsoFrameModel = &mdx_models[QHANDLETOINDEX_SAFE( refent->oldTorsoFrameModel,
																			   refent->torsoFrameModel )];
	mdx_t				*oldBoneFrameModel, *boneFrameModel;
	struct bone			*oldBone, *bone;
	struct frame_bone	*oldFrameBone, *frameBone;
	int					oldFrame, frame;
	float				backlerp;
	vec3_t				realangles, angles;
	vec3_t				axis1[3], tmpaxis[3];
	if ( frameModel->bones[idx].torso_weight )
	{
		boneFrameModel = torsoFrameModel;
		oldBoneFrameModel = oldTorsoFrameModel;
		frame = refent->torsoFrame;
		oldFrame = refent->oldTorsoFrame;
		backlerp = refent->torsoBacklerp;
	}
	else
	{
		boneFrameModel = frameModel;
		oldBoneFrameModel = oldFrameModel;
		frame = refent->frame;
		oldFrame = refent->oldframe;
		backlerp = refent->backlerp;
	}

	bone = &boneFrameModel->bones[idx];
	oldBone = &oldBoneFrameModel->bones[idx];

	if (frame < 0)
		frame = 0;

	if (oldFrame < 0)
		oldFrame = 0;

	frameBone = &boneFrameModel->frames[frame].bones[idx];
	oldFrameBone = &oldBoneFrameModel->frames[oldFrame].bones[idx];

	/* Calculate origin */
	VectorCopy( mdx_bones[idx], origin );

	/* Apply torso rotation to origin */

	// FIXME: This probably isn't entirely correct; my test models fail,
	// in any case.  It seems to produce the proper results with a real
	// player model, though.
	if ( bone->torso_weight )
	{
		vec3_t	tmp, torso_origin;

		/* Rotate around torso_parent */
		VectorSubtract( origin, mdx_bones[boneFrameModel->torso_parent], tmp );
		PointRotate( tmp, refent->torsoAxis, torso_origin );
		VectorAdd( torso_origin, mdx_bones[boneFrameModel->torso_parent], torso_origin );

		/* Lerp torso-rotated point with non-rotated */
		VectorSubtract( torso_origin, origin, torso_origin );
		VectorMA( origin, bone->torso_weight, torso_origin, origin );
	}

	/* Calculate angles */

	/* bone angles */
	realangles[0] = SHORT2ANGLE( oldFrameBone->angles[0] );
	realangles[1] = SHORT2ANGLE( oldFrameBone->angles[1] );
	realangles[2] = SHORT2ANGLE( oldFrameBone->angles[2] );
	VectorScale( realangles, backlerp, angles );
	realangles[0] = SHORT2ANGLE( frameBone->angles[0] );
	realangles[1] = SHORT2ANGLE( frameBone->angles[1] );
	realangles[2] = SHORT2ANGLE( frameBone->angles[2] );
	VectorMA( angles, (1.0 - backlerp), realangles, angles );
	AnglesToAxis( angles, tmpaxis );

	// FIXME: Why is this transpose needed?
	MatrixTranspose( tmpaxis, axis1 );

	/* torso angles */

	// FIXME: This probably isn't how the engine decides.
	MatrixWeight( refent->torsoAxis, bone->torso_weight, tmpaxis );
	MatrixMultiply( axis1, tmpaxis, axis );
}

#ifdef BONE_HITTESTS


/* */
static void
mdx_tag_orientation
	(	/*const*/
		grefEntity_t *refent,
		int idx,
		vec3_t origin,
		vec3_t axis[3],
		qboolean withhead,
		int recursion
	)
{
	int			i;
	vec3_t		tmpaxis[3];
	vec3_t		offset;
	mdm_t		*model;
	interntag_t *itag;
	struct tag	*tag;
	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	i = model->cachetags[idx];
	if ( i & TAG_INTERNAL )
	{
		itag = &interntags[i & TAG_INTERNAL_MASK];
		tag = &itag->tag;
	}
	else
	{
		itag = NULL;
		tag = &model->tags[i];
	}

	if ( tag->attach_bone & INTERNTAG_TAG )
	{
		mdx_tag_orientation( refent, tag->attach_bone & INTERNTAG_TAG_MASK, origin, tmpaxis, qfalse, recursion + 1 );
	}
	else
	{
		mdx_bone_orientation( refent, tag->attach_bone, origin, tmpaxis );
	}

	/* Apply head rotation, if this is a head tag */
	if ( itag && itag->ishead )
	{
		vec3_t	tmp[3];
		AxisCopy( tmpaxis, tmp );
		MatrixMultiply( refent->headAxis, tmp, tmpaxis );
	}

	/* Tag offset */
	PointRotate( tag->offset, tmpaxis, offset );
	VectorAdd( origin, offset, origin );

	/* Tag axis */
	MatrixMultiply( tag->axis, tmpaxis, axis );
	if ( !recursion )
	{
		VectorCopy( origin, offset );
		VectorCopy( refent->origin, origin );
		for ( i = 0; i < 3; i++ )
		{
			VectorMA( origin, offset[i], refent->axis[i], origin );
		}

		if ( withhead )
		{
			MatrixMultiply( refent->headAxis, axis, tmpaxis );
			MatrixMultiply( tmpaxis, refent->axis, axis );
		}
		else
		{
			MatrixMultiply( axis, refent->axis, tmpaxis );
			AxisCopy( tmpaxis, axis );
		}
	}

	if ( itag && itag->merged != -1 )
	{
		vec3_t	origin2;
		vec3_t	axis2[3];
		mdx_tag_orientation( refent, itag->merged, origin2, axis2, withhead, 0 );

		/* weighted origin */
		VectorSubtract( origin, origin2, origin );
		VectorMA( origin2, itag->weight, origin, origin );

		/* weighted axis */
		mdx_lerp_matrix( axis, axis2, tmpaxis, itag->weight );
		AxisCopy( tmpaxis, axis );
	}
}
#endif /* BONE_HITTESTS */

/* */
int
trap_R_LerpTagNumber ( orientation_t *tag, /*const*/ grefEntity_t *refent, int tagNum )
{
	mdm_t	*model;
	vec3_t	axis[3];
	vec3_t	offset;
	mdx_t	*mdx;
	int		bone;
	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	mdx = &mdx_models[QHANDLETOINDEX( refent->frameModel )];
	if ( tagNum < 0 || tagNum >= model->tag_count )
	{
		return ( -1 );
	}

	bone = model->tags[tagNum].attach_bone;
	mdx_calculate_bones_single( refent, bone );
	mdx_bone_orientation( refent, bone, tag->origin, axis );
	PointRotate( model->tags[tagNum].offset, axis, offset );
	VectorAdd( tag->origin, offset, tag->origin );
	MatrixMultiply( model->tags[tagNum].axis, axis, tag->axis );
	return ( 0 );
}


/* */
int
trap_R_LookupTag ( /*const*/ grefEntity_t *refent, const char *tagName )
{
	mdm_t	*model;
	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	return ( mdm_tag_lookup( model, tagName) );
}


/* */
int
trap_R_LerpTag ( orientation_t *tag, /*const*/ grefEntity_t *refent, const char *tagName, int startIndex )
{
	int tagNum;
	if ( startIndex )
	{
		G_Error( GAME_VERSION " MDX: Huh?  What to do, what to do... (non-zero startIndex)\n" );
	}

	tagNum = trap_R_LookupTag( refent, tagName );
	return ( trap_R_LerpTagNumber( tag, refent, tagNum) );
}

/**************************************************************/

/* Animations/Player stuff */
#define SWING_RIGHT 1
#define SWING_LEFT	2

/*
==================
mdx_SwingAngles, adapted from CG_SwingAngles
==================
*/
#define SERVER_FRAMETIME	( 1000 / 20 )


/* */
static void
mdx_SwingAngles
(
	float		destination,
	float		swingTolerance,
	float		clampTolerance,
	float		speed,
	float		*angle,
	qboolean	*swinging
)
{
	float	swing;
	float	move;
	float	scale;
	if ( !*swinging )
	{

		// see if a swing should be started
		float	centerAngle;

		// zinx - use predictable center so server can match cgame easier
		centerAngle = rint( *angle / swingTolerance ) * swingTolerance;
		swing = AngleSubtract( destination, *angle );
		if ( swing >= swingTolerance || swing < -swingTolerance )
		{
			*swinging = qtrue;
		}
	}

	if ( !*swinging )
	{
		return;
	}

	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = fabs( swing );
	scale *= 0.05;
	if ( scale < 0.5 )
	{
		scale = 0.5;
	}

	// swing towards the destination angle
	if ( swing >= 0 )
	{
		move = SERVER_FRAMETIME * scale * speed;
		if ( move >= swing )
		{
			move = swing;
			*swinging = qfalse;
		}
		else
		{
			*swinging = SWING_LEFT;		// left
		}

		*angle = AngleMod( *angle + move );
	}
	else if ( swing < 0 )
	{
		move = SERVER_FRAMETIME * scale * -speed;
		if ( move <= swing )
		{
			move = swing;
			*swinging = qfalse;
		}
		else
		{
			*swinging = SWING_RIGHT;	// right
		}

		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance )
	{
		*angle = AngleMod( destination - (clampTolerance - 1) );
	}
	else if ( swing < -clampTolerance )
	{
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}

#define POS_FW	0
#define POS_BC	1
#define POS_L	2
#define POS_R	3
#define POS_MAX 4


/* */
void
G_player_slope ( gentity_t *ent, vec3_t slopeangles )
{
	int		i;
	trace_t trace;
	vec3_t	forward, right, up, start, end;
	vec3_t	dir, angles;
	vec3_t	testpos[POS_MAX];
	vec3_t	testangles;
	vec3_t	boxMins = { -8, -8, -8 };
	vec3_t	boxMaxs = { 8, 8, 8 };
	float	pitch, roll;

	// do not take player viewangle in mind
	testangles[0] = testangles[2] = 0;
	testangles[1] = ent->client->ps.viewangles[1];
	AngleVectors( testangles, forward, right, up );
	for ( i = 0; i < POS_MAX; i++ )
	{
		float	fw, rt;
		fw = 16;
		rt = 8;
		VectorCopy( ent->s.pos.trBase, start );
		switch ( i )
		{
			case POS_FW:
				VectorMA( start, fw, forward, start );
				break;
			case POS_BC:
				VectorMA( start, -fw, forward, start );
				break;
			case POS_L:
				VectorMA( start, -rt, right, start );
				break;
			case POS_R:
				VectorMA( start, rt, right, start );
				break;
		}

		VectorMA( start, -32, up, end );
		VectorMA( start, 16, up, start );	// raise the start pos
		trap_Trace( &trace, start, boxMins, boxMaxs, end, ent->s.number, MASK_PLAYERSOLID );
		VectorCopy( trace.endpos, testpos[i] );
	}

	VectorSubtract( testpos[0], testpos[1], dir );
	vectoangles( dir, angles );
	pitch = angles[0];
	VectorSubtract( testpos[3], testpos[2], dir );
	vectoangles( dir, angles );
	roll = angles[0];
	slopeangles[0] = pitch;
	slopeangles[1] = 0;
	slopeangles[2] = roll;
}

/*
===============
mdx_PlayerAngles, adapted from CG_PlayerAngles
===============
*/
#define SWINGSPEED	0.1						/* Cheat protected, so we don't care if it matches. */


/* */
void
mdx_PlayerAngles ( gentity_t *ent, vec3_t legsAngles, vec3_t torsoAngles, vec3_t headAngles, qboolean doswing )
{
	float			dest;
	vec3_t			velocity;
	float			speed;
	float			clampTolerance;
	float			movementDir;
	bg_character_t	*character;
	int				legsSet;
	gclient_t		*client;
#ifdef __NPC__
	if ( ent->NPC_client )
	{
		client = ent->NPC_client;
	}
	else
	{
		client = ent->client;
	}

#else //!__NPC__
	client = ent->client;
#endif //__NPC__
	character = BG_GetCharacterForPlayerstate( &client->ps );
	if ( !character )
	{
		return;
	}

	legsSet = client->ps.legsAnim &~ANIM_TOGGLEBIT;
	if ( client->ps.movementDir > 128 )
	{
		movementDir = (float) client->ps.movementDir - 256;
	}
	else
	{
		movementDir = client->ps.movementDir;
	}

	VectorCopy( client->ps.viewangles, headAngles );
	headAngles[YAW] = AngleMod( headAngles[YAW] );
	VectorClear( legsAngles );
	VectorClear( torsoAngles );

	// --------- yaw -------------
	// allow yaw to drift a bit, unless these conditions don't allow them
	// rain - use clientNum instead of number, we get called for corpses.
	if
	(
		!(
			BG_GetConditionBitFlag( ent->s.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) ||
			BG_GetConditionBitFlag( ent->s.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLECR)
		)										/*
		||	 (BG_GetConditionValue( cent->currentState.clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFELEFT) | (1<<ANIM_MT_STRAFERIGHT)) )*/
	)
	{

		// always point all in the same direction
		ent->torsoFrame.yawing = qtrue;			// always center
		ent->torsoFrame.pitching = qtrue;		// always center
		ent->legsFrame.yawing = qtrue;			// always center

		// if firing, make sure torso and head are always aligned
		// rain - clientNum instead of number
	}
	else if ( BG_GetConditionValue( ent->s.clientNum, ANIM_COND_FIRING, qtrue) )
	{
		ent->torsoFrame.yawing = qtrue;			// always center
		ent->torsoFrame.pitching = qtrue;		// always center
	}

	// adjust legs for movement dir
	if ( client->ps.eFlags & EF_DEAD || client->ps.eFlags & EF_MOUNTEDTANK )
	{

		// don't let dead bodies twitch
		legsAngles[YAW] = headAngles[YAW];
		torsoAngles[YAW] = headAngles[YAW];
	}
	else
	{
		legsAngles[YAW] = headAngles[YAW] + movementDir;
		if ( !(client->ps.eFlags & EF_FIRING) )
		{
			torsoAngles[YAW] = headAngles[YAW] + 0.35 * movementDir;
			clampTolerance = 90;
		}
		else
		{										// must be firing
			torsoAngles[YAW] = headAngles[YAW]; // always face firing direction

			//if (fabs(ent->s.angles2[YAW]) > 30)
			//	legsAngles[YAW] = headAngles[YAW];
			clampTolerance = 60;
		}

		// torso
		if ( doswing )
		{
			mdx_SwingAngles( torsoAngles[YAW], 25, clampTolerance, SWINGSPEED, &ent->torsoFrame.yawAngle,
							 &ent->torsoFrame.yawing );
		}

		// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
		// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy
		clampTolerance = 150;
		if ( BG_GetConditionBitFlag( ent->s.clientNum, ANIM_COND_MOVETYPE, ANIM_MT_IDLE) )
		{
			if ( doswing )
			{
				ent->legsFrame.yawing = qfalse; // set it if they really need to swing
				mdx_SwingAngles( legsAngles[YAW], 20, clampTolerance, 0.5 * SWINGSPEED, &ent->legsFrame.yawAngle,
								 &ent->legsFrame.yawing );
			}
		}
		else if ( strstr( BG_GetAnimString( character->animModelInfo, legsSet), "strafe") )
		{

			// FIXME: what is this strstr hack??
			// if	( BG_GetConditionValue( ci->clientNum, ANIM_COND_MOVETYPE, qfalse ) & ((1<<ANIM_MT_STRAFERIGHT)|(1<<ANIM_MT_STRAFELEFT)) ) {
			// rain - this nasty strstr hack is to apply this only to
			// strafe animations, because strafing with some weapons uses
			// a non-strafe animation (!@#%), e.g. strafing with the
			// mobile mg42
			if ( doswing )
			{
				ent->legsFrame.yawing = qfalse; // set it if they really need to swing
				legsAngles[YAW] = headAngles[YAW];
				mdx_SwingAngles( legsAngles[YAW], 0, clampTolerance, SWINGSPEED, &ent->legsFrame.yawAngle,
								 &ent->legsFrame.yawing );
			}
		}
		else if ( ent->legsFrame.yawing )
		{
			if ( doswing )
			{
				mdx_SwingAngles( legsAngles[YAW], 0, clampTolerance, SWINGSPEED, &ent->legsFrame.yawAngle,
								 &ent->legsFrame.yawing );
			}
		}
		else
		{
			if ( doswing )
			{
				mdx_SwingAngles( legsAngles[YAW], 40, clampTolerance, SWINGSPEED, &ent->legsFrame.yawAngle,
								 &ent->legsFrame.yawing );
			}
		}

		torsoAngles[YAW] = ent->torsoFrame.yawAngle;
		legsAngles[YAW] = ent->legsFrame.yawAngle;
	}

	// --------- pitch -------------
	// only show a fraction of the pitch angle in the torso
	if ( headAngles[PITCH] > 180 )
	{
		dest = ( -360 + headAngles[PITCH] ) * 0.75;
	}
	else
	{
		dest = headAngles[PITCH] * 0.75;
	}

	// rain - zero out the head pitch when dead
	if ( client->ps.eFlags & EF_DEAD )
	{
		headAngles[PITCH] = 0;
	}

	//if (doswing) mdx_SwingAngles( dest, 15, 30, 0.1, &ent->torsoFrame.pitchAngle, &ent->torsoFrame.pitching );
	//torsoAngles[PITCH] = ent->torsoFrame.pitchAngle;
	if ( client->ps.eFlags & EF_PRONE )
	{
		torsoAngles[PITCH] = legsAngles[PITCH] - 3;
	}
	else if ( client->ps.eFlags & EF_DEAD )
	{

		// rain - zero out the torso pitch when dead
		torsoAngles[PITCH] = 0;
	}
	else
	{
		if ( doswing )
		{
			mdx_SwingAngles( dest, 15, 30, 0.1, &ent->torsoFrame.pitchAngle, &ent->torsoFrame.pitching );
		}

		torsoAngles[PITCH] = ent->torsoFrame.pitchAngle;
	}

	// --------- roll -------------
	// lean towards the direction of travel
	VectorCopy( client->ps.velocity, velocity );
	speed = VectorNormalize( velocity );
	if ( speed )
	{
		vec3_t	axis[3];
		float	side;
		speed *= 0.05;
		AnglesToAxis( legsAngles, axis );
		side = speed * DotProduct( velocity, axis[1] );
		legsAngles[ROLL] -= side;
		side = speed * DotProduct( velocity, axis[0] );
		legsAngles[PITCH] += side;
	}

	// FIXME: Do pain twitch?
	// jaquboss Lean bend	
	if ( client && client->ps.eFlags & EF_PRONE )
	{
		torsoAngles[ROLL] += client->ps.leanf * 2.3;
		headAngles[ROLL] += client->ps.leanf * 2.3;
	}
	else
	{
		torsoAngles[ROLL] += client->ps.leanf * 2.3;
		headAngles[ROLL] += client->ps.leanf * 2.3;
	}

	if ( client->ps.eFlags & EF_PRONE || client->ps.eFlags & EF_DEAD )
	{
		vec3_t	slope;
		G_player_slope( ent, slope );
		torsoAngles[0] += slope[0];
		headAngles[0] += slope[0];
		legsAngles[0] += slope[0];
		torsoAngles[2] += slope[2];
		headAngles[2] += slope[2];
		legsAngles[2] += slope[2];
	}

	// pull the angles back out of the hierarchial chain
	AnglesSubtract( headAngles, torsoAngles, headAngles );
	AnglesSubtract( torsoAngles, legsAngles, torsoAngles );
}

/* Adapted from CG_RunLerpFrameRate */

/* I'd rather not duplicate this much code.... */
#define CROUCHING( anim ) \
		( (anim) && ((anim)->movetype & ((1 << ANIM_MT_IDLECR) | (1 << ANIM_MT_WALKCR) | (1 << ANIM_MT_WALKCRBK))) )


/* */
static void
mdx_SetLerpFrame ( gentity_t *ent, glerpFrame_t *lf, int newAnimation, bg_character_t *character )
{
	animation_t *oldAnim;
	animation_t *anim;
	int			transitionMin = -1;
	qboolean	firstAnim = qfalse;
	if ( !lf->animation )
	{
		firstAnim = qtrue;
	}

	oldAnim = lf->animation;
	lf->animationNumber = newAnimation;
	anim = BG_GetAnimationForIndex( character->animModelInfo, lf->animationNumber &~ANIM_TOGGLEBIT );
	lf->animation = anim;
	lf->animationTime = lf->frameTime + anim->initialLerp;
	if ( !(anim->flags & ANIMFL_FIRINGANIM) || (lf != &ent->torsoFrame) )
	{
		if ( (lf == &ent->legsFrame) && (CROUCHING( oldAnim) != CROUCHING( anim)) )
		{
			if ( anim->moveSpeed || (anim->movetype & ((1 << ANIM_MT_TURNLEFT) | (1 << ANIM_MT_TURNRIGHT))) )
			{											// if unknown movetype, go there faster
				transitionMin = lf->frameTime + 200;	// slowly raise/drop
			}
			else
			{
				transitionMin = lf->frameTime + 350;	// slowly raise/drop
			}
		}
		else if ( anim->moveSpeed )
		{
			transitionMin = lf->frameTime + 120;		// always do some lerping (?)
		}
		else
		{												// not moving, so take your time
			transitionMin = lf->frameTime + 170;		// always do some lerping (?)
		}

		if ( oldAnim && oldAnim->animBlend )
		{												//transitionMin < lf->frameTime + oldanim->animBlend) {
			transitionMin = lf->frameTime + oldAnim->animBlend;
			lf->animationTime = transitionMin;
		}
		else
		{

			// slow down transitions according to speed
			if ( anim->moveSpeed && lf->animSpeedScale < 1.0 )
			{
				lf->animationTime += anim->initialLerp;
			}

			if ( lf->animationTime < transitionMin )
			{
				lf->animationTime = transitionMin;
			}
		}
	}

	// if first anim, go immediately
	if ( firstAnim )
	{
		lf->frameTime = level.time - 1;
		lf->animationTime = level.time - 1;
		lf->frame = anim->firstFrame;
		lf->frameModel = anim->mdxFile;
		VectorCopy( ent->s.pos.trBase, lf->oldFramePos );
	}

	//G_Printf("[fT%6d->%-6d] NA %d%s sT %d\n", lf->oldFrameTime, lf->frameTime, lf->animationNumber, firstAnim?" (FIRST)":"", level.time);
}


/* */
static void
mdx_RunLerpFrame ( gentity_t *ent, glerpFrame_t *lf, int newAnimation, bg_character_t *character, int recursion )
{
	int			f;
	animation_t *anim, *oldAnim;
	animation_t *otherAnim = NULL;
	qboolean	isLadderAnim;
	qboolean	done = qfalse;					// break out if we would loop forever
#define ANIM_SCALEMAX_LOW	1.1
#define ANIM_SCALEMAX_HIGH	1.6
#define ANIM_SPEEDMAX_LOW	100
#define ANIM_SPEEDMAX_HIGH	20
	isLadderAnim = lf->animation && ( lf->animation->flags & ANIMFL_LADDERANIM );
	oldAnim = lf->animation;
	if ( newAnimation != lf->animationNumber || !lf->animation )
	{
		mdx_SetLerpFrame( ent, lf, newAnimation, character );
	}

	anim = lf->animation;

	// check for forcing last frame
	if ( (ent->s.eFlags & EF_FORCE_END_FRAME) || ent->s.eType == ET_CORPSE )
	{
		lf->oldFrame = lf->frame = anim->firstFrame + anim->numFrames - 1;
		lf->oldFrameModel = lf->frameModel = anim->mdxFile;
		return;
	}

	// if we have passed the current frame, move it to
	// oldFrame and calculate a new frame
	while ( level.time > lf->frameTime && !done )
	{

		// Calculate move speed
		if ( lf->oldFrameSnapshotTime < ent->s.pos.trTime )
		{

			// We have a new snapshot, and thus a new speed
			float	serverDelta;

			// calculate the speed at which we moved over the last frame
			// zinx - changed to use server position instead of lerp'd position
			if ( isLadderAnim )
			{									// only use Z axis for speed
				lf->oldFramePos[0] = ent->s.pos.trBase[0];
				lf->oldFramePos[1] = ent->s.pos.trBase[1];
			}

			serverDelta = (float) ( ent->s.pos.trTime - lf->oldFrameSnapshotTime ) / 1000.0;
			lf->moveSpeed = Distance( ent->s.pos.trBase, lf->oldFramePos ) / serverDelta;
			VectorCopy( ent->s.pos.trBase, lf->oldFramePos );
			lf->oldFrameSnapshotTime = ent->s.pos.trTime;
		}

		// calculate speed for new frame
		if ( anim->moveSpeed )
		{

			// convert moveSpeed to a factor of this animation's movespeed
			lf->animSpeedScale = lf->moveSpeed / (float) anim->moveSpeed;
		}
		else
		{

			// move at normal speed
			lf->animSpeedScale = 1.0;
		}

		// restrict the speed range
		if ( lf->animSpeedScale < 0.25 )
		{										// if it's too slow, then a really slow spped, combined with a sudden take-off, can leave them playing a really slow frame while they a moving really fast
			if ( lf->animSpeedScale < 0.01 && isLadderAnim )
			{
				lf->animSpeedScale = 0.0;
			}
			else
			{
				lf->animSpeedScale = 0.25;
			}
		}
		else if ( lf->animSpeedScale > ANIM_SCALEMAX_LOW )
		{
			if ( !(anim->flags & ANIMFL_LADDERANIM) )
			{

				// allow slower anims to speed up more than faster anims
				if ( anim->moveSpeed > ANIM_SPEEDMAX_LOW )
				{
					lf->animSpeedScale = ANIM_SCALEMAX_LOW;
				}
				else if ( anim->moveSpeed < ANIM_SPEEDMAX_HIGH )
				{
					if ( lf->animSpeedScale > ANIM_SCALEMAX_HIGH )
					{
						lf->animSpeedScale = ANIM_SCALEMAX_HIGH;
					}
				}
				else
				{
					lf->animSpeedScale = ANIM_SCALEMAX_HIGH - ( ANIM_SCALEMAX_HIGH - ANIM_SCALEMAX_LOW ) * (float) ( anim->moveSpeed - ANIM_SPEEDMAX_HIGH ) / (float) ( ANIM_SPEEDMAX_LOW - ANIM_SPEEDMAX_HIGH );
				}
			}
			else if ( lf->animSpeedScale > 4.0 )
			{
				lf->animSpeedScale = 4.0;
			}
		}

		// move to new frame
		lf->oldFrame = lf->frame;
		lf->oldFrameTime = lf->frameTime;
		lf->oldFrameModel = lf->frameModel;
		if ( lf == &ent->legsFrame )
		{
			otherAnim = ent->torsoFrame.animation;
		}
		else if ( lf == &ent->torsoFrame )
		{
			otherAnim = ent->legsFrame.animation;
		}

		// get the next frame based on the animation
		if ( !lf->animSpeedScale )
		{

			// stopped on the ladder, so stay on the same frame
			f = lf->frame - anim->firstFrame;
			lf->frameTime += anim->frameLerp;	// don't wait too long before starting to move again
		}
		else if
			(
				lf->oldAnimationNumber != lf->animationNumber &&
				(
					!anim->moveSpeed ||
					lf->oldFrame < anim->firstFrame ||
					lf->oldFrame >= anim->firstFrame +
					anim->numFrames
				)
			)
		{										// Ridah, added this so walking frames don't always get reset to 0, which can happen in the middle of a walking anim, which looks wierd
			lf->frameTime = lf->animationTime;	// initial lerp
			if ( oldAnim && anim->moveSpeed )
			{									// keep locomotions going continuously
				f = ( lf->frame - oldAnim->firstFrame ) + 1;
				while ( f < 0 )
				{
					f += anim->numFrames;
				}
			}
			else
			{
				f = 0;
			}
		}
		else if
			(
				(lf == &ent->legsFrame) &&
				otherAnim &&
				!(anim->flags & ANIMFL_FIRINGANIM) &&
				((lf->animationNumber &~ANIM_TOGGLEBIT) == (ent->torsoFrame.animationNumber &~ANIM_TOGGLEBIT)) &&
				(!anim->moveSpeed)
			)
		{

			// legs should synch with torso
			f = ent->torsoFrame.frame - otherAnim->firstFrame;
			if ( f >= anim->numFrames || f < 0 )
			{
				f = 0;							// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}

			lf->frameTime = ent->torsoFrame.frameTime;
			done = qtrue;
		}
		else if
			(
				(lf == &ent->torsoFrame) &&
				otherAnim &&
				!(anim->flags & ANIMFL_FIRINGANIM) &&
				((lf->animationNumber &~ANIM_TOGGLEBIT) == (ent->legsFrame.animationNumber &~ANIM_TOGGLEBIT)) &&
				(otherAnim->moveSpeed)
			)
		{

			// torso needs to sync with legs
			f = ent->legsFrame.frame - otherAnim->firstFrame;
			if ( f >= anim->numFrames || f < 0 )
			{
				f = 0;							// wait at the start for the legs to catch up (assuming they are still in an old anim)
			}

			lf->frameTime = ent->legsFrame.frameTime;
			done = qtrue;
		}
		else
		{
			lf->frameTime = lf->oldFrameTime + (int) ( (float) anim->frameLerp * (1.0 / lf->animSpeedScale) );
			if ( anim->flags & ANIMFL_REVERSED )
			{
				f = ( anim->numFrames - 1 ) - ( (lf->frame - anim->firstFrame) - 1 );
			}
			else
			{
				f = ( lf->frame - anim->firstFrame ) + 1;
			}
		}

		//f = ( lf->frameTime - lf->animationTime ) / anim->frameLerp;
		if ( f >= anim->numFrames )
		{
			f -= anim->numFrames;
			if ( anim->loopFrames )
			{
				f %= anim->loopFrames;
				f += anim->numFrames - anim->loopFrames;
			}
			else
			{
				f = anim->numFrames - 1;

				// the animation is stuck at the end, so it
				// can immediately transition to another sequence
				lf->frameTime = level.time;
				done = qtrue;
			}
		}

		if ( anim->flags & ANIMFL_REVERSED )
		{
			lf->frame = anim->firstFrame + anim->numFrames - 1 - f;
			lf->frameModel = anim->mdxFile;
		}
		else
		{
			lf->frame = anim->firstFrame + f;
			lf->frameModel = anim->mdxFile;
		}

		lf->oldAnimationNumber = lf->animationNumber;
		oldAnim = anim;

		//G_Printf("[fT%6d->%-6d] NF %4d->%-4d s %1.4f sT %d\n", lf->oldFrameTime, lf->frameTime, lf->oldFrame, lf->frame, lf->animSpeedScale, level.time);
	}

	// Gordon: BIG hack, occaisionaly (VERY occaisionally), the frametime gets totally wacked
	if ( lf->frameTime > level.time + 5000 )
	{
		lf->frameTime = level.time;
	}
}


/* */
void
mdx_PlayerAnimation ( gentity_t *ent )
{
	bg_character_t	*character;
	vec3_t			legsAngles, torsoAngles, headAngles;
	int				animIndex, tempIndex;
#ifdef __NPC__
	if ( ent->s.eType == ET_NPC )
	{
		character = BG_GetCharacter( ent->s.teamNum, ent->playerType );
	}
	else
#endif __NPC__
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	animIndex = ent->s.legsAnim;

	// do the shuffle turn frames locally
	if ( !(ent->s.eFlags & EF_DEAD) && ent->legsFrame.yawing )
	{
		tempIndex = BG_GetAnimScriptAnimation( ent->s.number, character->animModelInfo, ent->s.aiState,
											   (ent->legsFrame.yawing == SWING_RIGHT ? ANIM_MT_TURNRIGHT : ANIM_MT_TURNLEFT) );
		if ( tempIndex > -1 )
		{
			animIndex = tempIndex;
		}
	}

	mdx_RunLerpFrame( ent, &ent->legsFrame, animIndex, character, 0 );
	mdx_RunLerpFrame( ent, &ent->torsoFrame, ent->s.torsoAnim, character, 0 );

	// swing angles
	mdx_PlayerAngles( ent, legsAngles, torsoAngles, headAngles, qtrue );
}

/**************************************************************/

/* Hit testing */
#ifdef BONE_HITTESTS


/* Rotates and transforms everything in to the space of origin/axis/scale,
   then finds the closest point to the origin on the line start<->end */
static qboolean
mdx_hit_warp
(
	const vec3_t	start,
	const vec3_t	end,
	const vec3_t	origin,
	/*const*/vec3_t axis[3],
	const vec3_t	scale,
	vec3_t			tangent,
	vec_t			*fraction
)
{
	vec3_t	unaxis[3];
	vec3_t	unstart, unend, undir;
	vec3_t	tmp;

	/* Un-translate */
	VectorSubtract( start, origin, unstart );
	VectorSubtract( end, origin, unend );

	/* Un-rotate */
	MatrixTranspose( axis, unaxis );
	PointRotate( unstart, unaxis, tmp );
	VectorCopy( tmp, unstart );
	PointRotate( unend, unaxis, tmp );
	VectorCopy( tmp, unend );

	/* Un-scale */
	unstart[0] /= scale[0];
	unstart[1] /= scale[1];
	unstart[2] /= scale[2];
	unend[0] /= scale[0];
	unend[1] /= scale[1];
	unend[2] /= scale[2];
	VectorSubtract( unend, unstart, undir );

	/* Find closest point */
	VectorNegate( unstart, unstart );
	*fraction = DotProduct( unstart, undir ) / DotProduct( undir, undir );
	VectorNegate( unstart, unstart );
	if ( *fraction < 0.0 )
	{
		VectorCopy( unstart, tangent );
		*fraction = 0;
		return ( qfalse );	// don't shoot backwards..
	}
	else if ( *fraction >= 1.0 )
	{
		VectorCopy( unend, tangent );
		*fraction = 1.0;
	}
	else
	{
		VectorMA( unstart, *fraction, undir, tangent );
	}

	if ( g_debugBullets.integer >= 4 )
	{

		/* Draw line from tangent point */
		vec3_t	point;
		VectorSubtract( end, start, point );
		VectorMA( start, *fraction, point, point );

		//		etpro_AddDebugLine(origin, point, 2, LINEMODE_LINE, LINESHADER_RAILCORE, 0, qfalse);
	}

	return ( qtrue );
}


/* */
static qboolean
mdx_hit_test_cylinder ( const vec3_t p1, const vec3_t p2, float *backlerp )
{
	vec_t	lerpt, lerp1, lerp2;
	vec_t	distx, disty;

	/* Check ends */
	if ( p1[2] < 0 )
	{
		return ( qfalse );
	}

	if ( p2[2] > 0 )
	{
		return ( qfalse );
	}

	/* Check radius */
	lerpt = p1[2] + -p2[2];
	lerp1 = ( lerpt - p1[2] ) / lerpt;
	lerp2 = 1.0 - lerp1;
	*backlerp = lerp1;
	distx = p1[0] * lerp1 + p2[0] * lerp2;
	distx *= distx;
	disty = p1[1] * lerp1 + p2[1] * lerp2;
	disty *= disty;
	if ( (distx + disty) > 1.0 )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/* */
static qboolean
mdx_hit_test_box2 ( const vec3_t p1, const vec3_t p2, float *backlerp )
{
	vec_t	lerpt, lerp1, lerp2;
	vec_t	distx, disty;

	/* Check ends */
	if ( p1[2] < 0 )
	{
		return ( qfalse );
	}

	if ( p2[2] > 0 )
	{
		return ( qfalse );
	}

	/* Check radius */
	lerpt = p1[2] + -p2[2];
	lerp1 = ( lerpt - p1[2] ) / lerpt;
	lerp2 = 1.0 - lerp1;
	*backlerp = lerp1;
	distx = p1[0] * lerp1 + p2[0] * lerp2;
	if ( Q_fabs( distx) > 1.0 )
	{
		return ( qfalse );
	}

	disty = p1[1] * lerp1 + p2[1] * lerp2;
	if ( Q_fabs( disty) > 1.0 )
	{
		return ( qfalse );
	}

	return ( qtrue );
}


/* */
static qboolean
mdx_hit_test_sphere ( const vec3_t p1 )
{

	/* Check radius */
	if ( VectorLengthSquared( p1) > Square( 1.0) )
	{
		return ( qfalse );
	}

	/* Hit */
	return ( qtrue );
}


/* */
static qboolean
mdx_hit_test_box ( const vec3_t p1 )
{

	/* Check radius */
	if ( Q_fabs( p1[0]) > 1.0 )
	{
		return ( qfalse );
	}

	if ( Q_fabs( p1[1]) > 1.0 )
	{
		return ( qfalse );
	}

	if ( Q_fabs( p1[2]) > 1.0 )
	{
		return ( qfalse );
	}

	/* Hit */
	return ( qtrue );
}


/* */
qboolean
mdx_hit_test
(
	const vec3_t			start,
	const vec3_t			end,	/*const*/
	gentity_t				*ent,	/*const*/
	grefEntity_t			*refent,
	int						*hit_type,
	vec_t					*fraction,
	animScriptImpactPoint_t *impactpoint
)
{
	int						i;
	bg_character_t			*character;
	hit_t					*hitModel;
	vec_t					best_frac;
	int						best_type;
	animScriptImpactPoint_t best_impactpoint;
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	// FIXME: Need better lookup for this eventually
	for ( i = 0; i < hit_count; i++ )
	{
		if ( hits[i].animModelInfo == character->animModelInfo )
		{
			break;
		}
	}

	if ( i >= hit_count )
	{
		return ( qfalse );
	}

	hitModel = &hits[i];
	mdx_calculate_bones( refent );
	best_type = MDX_NONE;
	best_frac = 2.0;
	best_impactpoint = IMPACTPOINT_UNUSED;
	for ( i = 0; i < hitModel->hit_count; i++ )
	{
		struct hit_area *hit = &hitModel->hits[i];
		vec_t			hit_frac;
		vec3_t			o1, o2;
		vec3_t			a1[3], a2[3];
		vec3_t			t1, t2;
		mdx_tag_orientation( refent, hit->tag[0], o1, a1, hit->ishead[0], 0 );
		if ( hit->tag[1] != -1 )
		{
			vec_t	hit_frac1, hit_frac2;
			mdx_tag_orientation( refent, hit->tag[1], o2, a2, hit->ishead[1], 0 );

			/* Calculate axis */
			VectorSubtract( o2, o1, a1[2] );
			VectorNormalize( a1[2] );
			CrossProduct( a1[2], a1[0], a1[1] );
			VectorNormalize( a1[1] );
			CrossProduct( a1[2], a1[1], a1[0] );

			/* Apply hit axis */
			MatrixMultiply( a1, hit->axis, a2 );

			/*		if (g_debugBullets.integer >= 3) {
				etpro_AddDebugLine(o1, o2, 1, LINEMODE_LINE, LINESHADER_RAILCORE, 0, qfalse);

				VectorScale(a2[0], hit->scale[0][0], a1[0]);
				VectorScale(a2[1], hit->scale[0][1], a1[1]);
				VectorScale(a2[2], hit->scale[0][2], a1[2]);
#ifdef DEBUG
				etpro_AddDebugLine(o1, a1[0], 1, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
				etpro_AddDebugLine(o1, a1[1], 2, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
				etpro_AddDebugLine(o1, a1[2], 3, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
#endif

				VectorScale(a2[0], hit->scale[1][0], a1[0]);
				VectorScale(a2[1], hit->scale[1][1], a1[1]);
				VectorScale(a2[2], hit->scale[1][2], a1[2]);
#ifdef DEBUG
				etpro_AddDebugLine(o2, a1[0], 1, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
				etpro_AddDebugLine(o2, a1[1], 2, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
				etpro_AddDebugLine(o2, a1[2], 3, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
#endif
			}*/
			if ( !mdx_hit_warp( start, end, o1, a2, hit->scale[0], t1, &hit_frac1) )
			{
				continue;
			}

			if ( !mdx_hit_warp( start, end, o2, a2, hit->scale[1], t2, &hit_frac2) )
			{
				continue;
			}

			if ( hit->isbox )
			{
				if ( !mdx_hit_test_box2( t1, t2, &hit_frac) )
				{
					continue;
				}
			}
			else
			{
				if ( !mdx_hit_test_cylinder( t1, t2, &hit_frac) )
				{
					continue;
				}
			}

			hit_frac = hit_frac1 * hit_frac + hit_frac2 * ( 1.0 - hit_frac );
		}
		else
		{

			/* Apply hit axis */
			MatrixMultiply( a1, hit->axis, a2 );

			/*			if (g_debugBullets.integer >= 3) {
				VectorScale(a2[0], hit->scale[0][0], a1[0]);
				VectorScale(a2[1], hit->scale[0][1], a1[1]);
				VectorScale(a2[2], hit->scale[0][2], a1[2]);
#ifdef DEBUG
				etpro_AddDebugLine(o1, a1[0], 1, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
				etpro_AddDebugLine(o1, a1[1], 2, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
				etpro_AddDebugLine(o1, a1[2], 3, LINEMODE_ARROW, LINESHADER_RAILCORE, 0, qfalse);
#endif
			}
*/
			if ( !mdx_hit_warp( start, end, o1, a2, hit->scale[0], t1, &hit_frac) )
			{
				continue;
			}

			if ( hit->isbox )
			{
				if ( !mdx_hit_test_box( t1) )
				{
					continue;
				}
			}
			else
			{
				if ( !mdx_hit_test_sphere( t1) )
				{
					continue;
				}
			}
		}

		/* It hit. */
		if ( best_frac > hit_frac )
		{
			best_type = hit->hit_type;
			best_frac = hit_frac;
			best_impactpoint = hit->impactpoint;
		}
	}

	if ( best_frac > 1.0 )
	{
		best_frac = 1.0;
	}

	if ( hit_type )
	{
		*hit_type = best_type;
	}

	if ( fraction )
	{
		*fraction = best_frac;
	}

	if ( impactpoint )
	{
		*impactpoint = best_impactpoint;
	}

	return ( qtrue );
}
#endif

/* For new old-style hit tests; returns -center- positions, to have -centered- bbox applied. */
void
mdx_head_position ( /*const*/ gentity_t *ent, /*const*/ grefEntity_t *refent, vec3_t org )
{
	bg_character_t	*character;
	mdm_t			*model;
	orientation_t	o;
	vec3_t			axis[3];
	int				i;
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	trap_R_LerpTagNumber( &o, refent, model->tag_head );

	/* Tag offset */
	VectorCopy( refent->origin, org );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org, o.origin[i], refent->axis[i], org );
	}

	/* Apply head/body rotation */
	MatrixMultiply( refent->headAxis, o.axis, axis );
	MatrixMultiply( axis, refent->axis, o.axis );

	/* calculate center position for standard head (this offset is just a guess) */
	VectorMA( org, 6.5, o.axis[2], org );	// up
	VectorMA( org, 0.5, o.axis[0], org );	// forward
}


/* */
void
mdx_body_position ( /*const*/ gentity_t *ent, /*const*/ grefEntity_t *refent, vec3_t org )
{
	bg_character_t	*character;
	mdm_t			*model;
	orientation_t	o;
	vec3_t			axis[3];
	int				i;
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	trap_R_LerpTagNumber( &o, refent, model->tag_chest );

	/* Tag offset */
	VectorCopy( refent->origin, org );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org, o.origin[i], refent->axis[i], org );
	}

	/* Apply head/body rotation */
	MatrixMultiply( refent->torsoAxis, o.axis, axis );
	MatrixMultiply( axis, refent->axis, o.axis );

	/* calculate center position for standard head (this offset is just a guess) */
	VectorMA( org, 6.5, o.axis[2], org );	// up
	VectorMA( org, 0.5, o.axis[0], org );	// forward
}


/* */
void
mdx_groin_position ( /*const*/ gentity_t *ent, /*const*/ grefEntity_t *refent, vec3_t org )
{
	bg_character_t	*character;
	mdm_t			*model;
	orientation_t	o;
	vec3_t			axis[3];
	int				i;
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	trap_R_LerpTagNumber( &o, refent, model->tag_ubelt );

	/* Tag offset */
	VectorCopy( refent->origin, org );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org, o.origin[i], refent->axis[i], org );
	}

	/* Apply head/body rotation */
	MatrixMultiply( refent->torsoAxis, o.axis, axis );
	MatrixMultiply( axis, refent->axis, o.axis );

	/* calculate center position for standard head (this offset is just a guess) */
	VectorMA( org, 6.5, o.axis[2], org );	// up
	VectorMA( org, 0.5, o.axis[0], org );	// forward
}


/* */
void
mdx_legs_position ( /*const*/ gentity_t *ent, /*const*/ grefEntity_t *refent, vec3_t org )
{
	bg_character_t	*character;
	mdm_t			*model;
	orientation_t	o;
	vec3_t			org1, org2;
	int				i;
	if ( ent->s.eType == ET_PLAYER )
	{
		character = BG_GetCharacter( ent->client->sess.sessionTeam, ent->client->sess.playerType );
	}
	else
	{
		character = BG_GetCharacter( BODY_TEAM( ent), BODY_CLASS( ent) );
	}

	model = &mdm_models[QHANDLETOINDEX( refent->hModel )];
	trap_R_LerpTagNumber( &o, refent, model->tag_footleft );

	/* Tag offset */
	VectorCopy( refent->origin, org1 );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org1, o.origin[i], refent->axis[i], org1 );
	}

	trap_R_LerpTagNumber( &o, refent, model->tag_footright );

	/* Tag offset */
	VectorCopy( refent->origin, org2 );
	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org2, o.origin[i], refent->axis[i], org2 );
	}

	VectorAdd( org1, org2, org );
	VectorScale( org, 0.5, org );
}
