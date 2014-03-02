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


// UQ1: Various procedures for using model TAG...
#include "cg_local.h"


/*
===============
CG_TagValid
===============
*/
qboolean
CG_TagValid ( refEntity_t refent, char *tagname )
{
	orientation_t	or;
	if ( trap_R_LerpTag( &or, &refent, tagname, 0) < 0 )
	{
		return ( qfalse );
	}

	return ( qtrue );
}

/*
===============
CG_GetTagOrigin
===============
*/
vec3_t	tag_origin;
vec3_t	tag_axis[3];


/* */
float *
CG_GetTagOrigin ( refEntity_t refent, char *tagname, vec3_t *offset )
{
	orientation_t	or;
	vec3_t			org;
	int				i;
	VectorSet( tag_origin, 0, 0, 0 );
	if ( trap_R_LerpTag( &or, &refent, tagname, 0) < 0 )
	{
		return ( tag_origin );
	}

	VectorCopy( refent.origin, org );
	if ( offset )
	{
		VectorAdd( org, *offset, org );
	}

	for ( i = 0; i < 3; i++ )
	{
		VectorMA( org, or.origin[i], refent.axis[i], org );
	}

	// had to cast away the const to avoid compiler problems...
	MatrixMultiply( or.axis, refent.axis, tag_axis );
	VectorCopy( org, or.origin );
	VectorCopy( or.origin, tag_origin );
	return ( tag_origin );
}
