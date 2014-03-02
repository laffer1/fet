#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "gltrace.h"
#include "dll.h"
#include "input.h"
#include "WINBASE.h"

/* Misc. globals */
extern int		GetEXTProcIndex ( LPCSTR name );
static void		*OpenGL_provider;
struct timeb	starttime;								/* Time when trace started */
FILE			*text_file = NULL;						/* Output text file (NULL for stdout) */
GLboolean		needInit = GL_TRUE;
char			*GLenum_names[65536];					/* Pointer to text string for each GLenum value */
GLfloat			GLT_WHITE[3] = { 1.0f, 1.0f, 1.0f };	//Static
GLfloat			GLT_BLACK[3] = { 0.0f, 0.0f, 0.0f };	//Static
GLfloat			GLT_YELLOW[3] = { 1.0f, 1.0f, 0.0f };	//Static
GLfloat			GLT_GRAY[] = { 0.5, 0.5, 0.5, 0.0 };	//Static
GLfloat			GLT_RED[] = { 1.0f, 0.0f, 0.0f, 0.0f }; //Static
GLfloat			GLT_TEST[] = { 0.4, 0.5, 0.5, 1.0 };	//Static
GLboolean		useZeroOne = GL_FALSE;					/* To correct GL_ZERO and GL_ONE overlap with other enumerants */
GLboolean		bVerbose = GL_FALSE;					/* Controls the amount of info written to log */
GLboolean		bCountCalls = GL_FALSE;					/* count the number of function calls */
GLboolean		bLogTime = GL_FALSE;					/* log the time of each function call */
GLboolean		bFSPenable = GL_FALSE;					/* enable the FSP counter */
GLboolean		bLogEnable = GL_FALSE;					/* enable normal logging */
GLfloat			*fpsColor = GLT_GRAY;					/* color of the FPS counter */

/*********************************/

/*          For Time             */

/*********************************/
GLfloat			*timeColor = GLT_YELLOW;				//Static
GLboolean		bTimeEnabled = GL_TRUE;					//Static

/* End Time */

/*********************************/

/*       For Tanbs Walls         */

/*********************************/
GLboolean		bTransEnabled = GL_FALSE;

/* End Trans */
GL_ENUM_PAIR	GLenum_list[] =
{
	{ "GL_POINTS", 0x0000 },
	{ "GL_LINES", 0x0001 },
	{ "GL_LINE_LOOP", 0x0002 },
	{ "GL_LINE_STRIP", 0x0003 },
	{ "GL_TRIANGLES", 0x0004 },
	{ "GL_TRIANGLE_STRIP", 0x0005 },
	{ "GL_TRIANGLE_FAN", 0x0006 },
	{ "GL_QUADS", 0x0007 },
	{ "GL_QUAD_STRIP", 0x0008 },
	{ "GL_POLYGON", 0x0009 },
	{ "GL_ACCUM", 0x0100 },
	{ "GL_LOAD", 0x0101 },
	{ "GL_RETURN", 0x0102 },
	{ "GL_MULT", 0x0103 },
	{ "GL_ADD", 0x0104 },
	{ "GL_NEVER", 0x0200 },
	{ "GL_LESS", 0x0201 },
	{ "GL_EQUAL", 0x0202 },
	{ "GL_LEQUAL", 0x0203 },
	{ "GL_GREATER", 0x0204 },
	{ "GL_NOTEQUAL", 0x0205 },
	{ "GL_GEQUAL", 0x0206 },
	{ "GL_ALWAYS", 0x0207 },
	{ "GL_SRC_COLOR", 0x0300 },
	{ "GL_ONE_MINUS_SRC_COLOR", 0x0301 },
	{ "GL_SRC_ALPHA", 0x0302 },
	{ "GL_ONE_MINUS_SRC_ALPHA", 0x0303 },
	{ "GL_DST_ALPHA", 0x0304 },
	{ "GL_ONE_MINUS_DST_ALPHA", 0x0305 },
	{ "GL_DST_COLOR", 0x0306 },
	{ "GL_ONE_MINUS_DST_COLOR", 0x0307 },
	{ "GL_SRC_ALPHA_SATURATE", 0x0308 },
	{ "GL_FRONT_LEFT", 0x0400 },
	{ "GL_FRONT_RIGHT", 0x0401 },
	{ "GL_BACK_LEFT", 0x0402 },
	{ "GL_BACK_RIGHT", 0x0403 },
	{ "GL_FRONT", 0x0404 },
	{ "GL_BACK", 0x0405 },
	{ "GL_LEFT", 0x0406 },
	{ "GL_RIGHT", 0x0407 },
	{ "GL_FRONT_AND_BACK", 0x0408 },
	{ "GL_AUX0", 0x0409 },
	{ "GL_AUX1", 0x040A },
	{ "GL_AUX2", 0x040B },
	{ "GL_AUX3", 0x040C },
	{ "GL_INVALID_ENUM", 0x0500 },
	{ "GL_INVALID_VALUE", 0x0501 },
	{ "GL_INVALID_OPERATION", 0x0502 },
	{ "GL_STACK_OVERFLOW", 0x0503 },
	{ "GL_STACK_UNDERFLOW", 0x0504 },
	{ "GL_OUT_OF_MEMORY", 0x0505 },
	{ "GL_2D", 0x0600 },
	{ "GL_3D", 0x0601 },
	{ "GL_3D_COLOR", 0x0602 },
	{ "GL_3D_COLOR_TEXTURE", 0x0603 },
	{ "GL_4D_COLOR_TEXTURE", 0x0604 },
	{ "GL_PASS_THROUGH_TOKEN", 0x0700 },
	{ "GL_POINT_TOKEN", 0x0701 },
	{ "GL_LINE_TOKEN", 0x0702 },
	{ "GL_POLYGON_TOKEN", 0x0703 },
	{ "GL_BITMAP_TOKEN", 0x0704 },
	{ "GL_DRAW_PIXEL_TOKEN", 0x0705 },
	{ "GL_COPY_PIXEL_TOKEN", 0x0706 },
	{ "GL_LINE_RESET_TOKEN", 0x0707 },
	{ "GL_EXP", 0x0800 },
	{ "GL_EXP2", 0x0801 },
	{ "GL_CW", 0x0900 },
	{ "GL_CCW", 0x0901 },
	{ "GL_COEFF", 0x0A00 },
	{ "GL_ORDER", 0x0A01 },
	{ "GL_DOMAIN", 0x0A02 },
	{ "GL_CURRENT_COLOR", 0x0B00 },
	{ "GL_CURRENT_INDEX", 0x0B01 },
	{ "GL_CURRENT_NORMAL", 0x0B02 },
	{ "GL_CURRENT_TEXTURE_COORDS", 0x0B03 },
	{ "GL_CURRENT_RASTER_COLOR", 0x0B04 },
	{ "GL_CURRENT_RASTER_INDEX", 0x0B05 },
	{ "GL_CURRENT_RASTER_TEXTURE_COORDS", 0x0B06 },
	{ "GL_CURRENT_RASTER_POSITION", 0x0B07 },
	{ "GL_CURRENT_RASTER_POSITION_VALID", 0x0B08 },
	{ "GL_CURRENT_RASTER_DISTANCE", 0x0B09 },
	{ "GL_POINT_SMOOTH", 0x0B10 },
	{ "GL_POINT_SIZE", 0x0B11 },
	{ "GL_POINT_SIZE_RANGE", 0x0B12 },
	{ "GL_POINT_SIZE_GRANULARITY", 0x0B13 },
	{ "GL_LINE_SMOOTH", 0x0B20 },
	{ "GL_LINE_WIDTH", 0x0B21 },
	{ "GL_LINE_WIDTH_RANGE", 0x0B22 },
	{ "GL_LINE_WIDTH_GRANULARITY", 0x0B23 },
	{ "GL_LINE_STIPPLE", 0x0B24 },
	{ "GL_LINE_STIPPLE_PATTERN", 0x0B25 },
	{ "GL_LINE_STIPPLE_REPEAT", 0x0B26 },
	{ "GL_LIST_MODE", 0x0B30 },
	{ "GL_MAX_LIST_NESTING", 0x0B31 },
	{ "GL_LIST_BASE", 0x0B32 },
	{ "GL_LIST_INDEX", 0x0B33 },
	{ "GL_POLYGON_MODE", 0x0B40 },
	{ "GL_POLYGON_SMOOTH", 0x0B41 },
	{ "GL_POLYGON_STIPPLE", 0x0B42 },
	{ "GL_EDGE_FLAG", 0x0B43 },
	{ "GL_CULL_FACE", 0x0B44 },
	{ "GL_CULL_FACE_MODE", 0x0B45 },
	{ "GL_FRONT_FACE", 0x0B46 },
	{ "GL_LIGHTING", 0x0B50 },
	{ "GL_LIGHT_MODEL_LOCAL_VIEWER", 0x0B51 },
	{ "GL_LIGHT_MODEL_TWO_SIDE", 0x0B52 },
	{ "GL_LIGHT_MODEL_AMBIENT", 0x0B53 },
	{ "GL_SHADE_MODEL", 0x0B54 },
	{ "GL_COLOR_MATERIAL_FACE", 0x0B55 },
	{ "GL_COLOR_MATERIAL_PARAMETER", 0x0B56 },
	{ "GL_COLOR_MATERIAL", 0x0B57 },
	{ "GL_FOG", 0x0B60 },
	{ "GL_FOG_INDEX", 0x0B61 },
	{ "GL_FOG_DENSITY", 0x0B62 },
	{ "GL_FOG_START", 0x0B63 },
	{ "GL_FOG_END", 0x0B64 },
	{ "GL_FOG_MODE", 0x0B65 },
	{ "GL_FOG_COLOR", 0x0B66 },
	{ "GL_DEPTH_RANGE", 0x0B70 },
	{ "GL_DEPTH_TEST", 0x0B71 },
	{ "GL_DEPTH_WRITEMASK", 0x0B72 },
	{ "GL_DEPTH_CLEAR_VALUE", 0x0B73 },
	{ "GL_DEPTH_FUNC", 0x0B74 },
	{ "GL_ACCUM_CLEAR_VALUE", 0x0B80 },
	{ "GL_STENCIL_TEST", 0x0B90 },
	{ "GL_STENCIL_CLEAR_VALUE", 0x0B91 },
	{ "GL_STENCIL_FUNC", 0x0B92 },
	{ "GL_STENCIL_VALUE_MASK", 0x0B93 },
	{ "GL_STENCIL_FAIL", 0x0B94 },
	{ "GL_STENCIL_PASS_DEPTH_FAIL", 0x0B95 },
	{ "GL_STENCIL_PASS_DEPTH_PASS", 0x0B96 },
	{ "GL_STENCIL_REF", 0x0B97 },
	{ "GL_STENCIL_WRITEMASK", 0x0B98 },
	{ "GL_MATRIX_MODE", 0x0BA0 },
	{ "GL_NORMALIZE", 0x0BA1 },
	{ "GL_VIEWPORT", 0x0BA2 },
	{ "GL_MODELVIEW_STACK_DEPTH", 0x0BA3 },
	{ "GL_PROJECTION_STACK_DEPTH", 0x0BA4 },
	{ "GL_TEXTURE_STACK_DEPTH", 0x0BA5 },
	{ "GL_MODELVIEW_MATRIX", 0x0BA6 },
	{ "GL_PROJECTION_MATRIX", 0x0BA7 },
	{ "GL_TEXTURE_MATRIX", 0x0BA8 },
	{ "GL_ATTRIB_STACK_DEPTH", 0x0BB0 },
	{ "GL_CLIENT_ATTRIB_STACK_DEPTH", 0x0BB1 },
	{ "GL_ALPHA_TEST", 0x0BC0 },
	{ "GL_ALPHA_TEST_FUNC", 0x0BC1 },
	{ "GL_ALPHA_TEST_REF", 0x0BC2 },
	{ "GL_DITHER", 0x0BD0 },
	{ "GL_BLEND_DST", 0x0BE0 },
	{ "GL_BLEND_SRC", 0x0BE1 },
	{ "GL_BLEND", 0x0BE2 },
	{ "GL_LOGIC_OP_MODE", 0x0BF0 },
	{ "GL_INDEX_LOGIC_OP", 0x0BF1 },
	{ "GL_COLOR_LOGIC_OP", 0x0BF2 },
	{ "GL_AUX_BUFFERS", 0x0C00 },
	{ "GL_DRAW_BUFFER", 0x0C01 },
	{ "GL_READ_BUFFER", 0x0C02 },
	{ "GL_SCISSOR_BOX", 0x0C10 },
	{ "GL_SCISSOR_TEST", 0x0C11 },
	{ "GL_INDEX_CLEAR_VALUE", 0x0C20 },
	{ "GL_INDEX_WRITEMASK", 0x0C21 },
	{ "GL_COLOR_CLEAR_VALUE", 0x0C22 },
	{ "GL_COLOR_WRITEMASK", 0x0C23 },
	{ "GL_INDEX_MODE", 0x0C30 },
	{ "GL_RGBA_MODE", 0x0C31 },
	{ "GL_DOUBLEBUFFER", 0x0C32 },
	{ "GL_STEREO", 0x0C33 },
	{ "GL_RENDER_MODE", 0x0C40 },
	{ "GL_PERSPECTIVE_CORRECTION_HINT", 0x0C50 },
	{ "GL_POINT_SMOOTH_HINT", 0x0C51 },
	{ "GL_LINE_SMOOTH_HINT", 0x0C52 },
	{ "GL_POLYGON_SMOOTH_HINT", 0x0C53 },
	{ "GL_FOG_HINT", 0x0C54 },
	{ "GL_TEXTURE_GEN_S", 0x0C60 },
	{ "GL_TEXTURE_GEN_T", 0x0C61 },
	{ "GL_TEXTURE_GEN_R", 0x0C62 },
	{ "GL_TEXTURE_GEN_Q", 0x0C63 },
	{ "GL_PIXEL_MAP_I_TO_I", 0x0C70 },
	{ "GL_PIXEL_MAP_S_TO_S", 0x0C71 },
	{ "GL_PIXEL_MAP_I_TO_R", 0x0C72 },
	{ "GL_PIXEL_MAP_I_TO_G", 0x0C73 },
	{ "GL_PIXEL_MAP_I_TO_B", 0x0C74 },
	{ "GL_PIXEL_MAP_I_TO_A", 0x0C75 },
	{ "GL_PIXEL_MAP_R_TO_R", 0x0C76 },
	{ "GL_PIXEL_MAP_G_TO_G", 0x0C77 },
	{ "GL_PIXEL_MAP_B_TO_B", 0x0C78 },
	{ "GL_PIXEL_MAP_A_TO_A", 0x0C79 },
	{ "GL_PIXEL_MAP_I_TO_I_SIZE", 0x0CB0 },
	{ "GL_PIXEL_MAP_S_TO_S_SIZE", 0x0CB1 },
	{ "GL_PIXEL_MAP_I_TO_R_SIZE", 0x0CB2 },
	{ "GL_PIXEL_MAP_I_TO_G_SIZE", 0x0CB3 },
	{ "GL_PIXEL_MAP_I_TO_B_SIZE", 0x0CB4 },
	{ "GL_PIXEL_MAP_I_TO_A_SIZE", 0x0CB5 },
	{ "GL_PIXEL_MAP_R_TO_R_SIZE", 0x0CB6 },
	{ "GL_PIXEL_MAP_G_TO_G_SIZE", 0x0CB7 },
	{ "GL_PIXEL_MAP_B_TO_B_SIZE", 0x0CB8 },
	{ "GL_PIXEL_MAP_A_TO_A_SIZE", 0x0CB9 },
	{ "GL_UNPACK_SWAP_BYTES", 0x0CF0 },
	{ "GL_UNPACK_LSB_FIRST", 0x0CF1 },
	{ "GL_UNPACK_ROW_LENGTH", 0x0CF2 },
	{ "GL_UNPACK_SKIP_ROWS", 0x0CF3 },
	{ "GL_UNPACK_SKIP_PIXELS", 0x0CF4 },
	{ "GL_UNPACK_ALIGNMENT", 0x0CF5 },
	{ "GL_PACK_SWAP_BYTES", 0x0D00 },
	{ "GL_PACK_LSB_FIRST", 0x0D01 },
	{ "GL_PACK_ROW_LENGTH", 0x0D02 },
	{ "GL_PACK_SKIP_ROWS", 0x0D03 },
	{ "GL_PACK_SKIP_PIXELS", 0x0D04 },
	{ "GL_PACK_ALIGNMENT", 0x0D05 },
	{ "GL_MAP_COLOR", 0x0D10 },
	{ "GL_MAP_STENCIL", 0x0D11 },
	{ "GL_INDEX_SHIFT", 0x0D12 },
	{ "GL_INDEX_OFFSET", 0x0D13 },
	{ "GL_RED_SCALE", 0x0D14 },
	{ "GL_RED_BIAS", 0x0D15 },
	{ "GL_ZOOM_X", 0x0D16 },
	{ "GL_ZOOM_Y", 0x0D17 },
	{ "GL_GREEN_SCALE", 0x0D18 },
	{ "GL_GREEN_BIAS", 0x0D19 },
	{ "GL_BLUE_SCALE", 0x0D1A },
	{ "GL_BLUE_BIAS", 0x0D1B },
	{ "GL_ALPHA_SCALE", 0x0D1C },
	{ "GL_ALPHA_BIAS", 0x0D1D },
	{ "GL_DEPTH_SCALE", 0x0D1E },
	{ "GL_DEPTH_BIAS", 0x0D1F },
	{ "GL_MAX_EVAL_ORDER", 0x0D30 },
	{ "GL_MAX_LIGHTS", 0x0D31 },
	{ "GL_MAX_CLIP_PLANES", 0x0D32 },
	{ "GL_MAX_TEXTURE_SIZE", 0x0D33 },
	{ "GL_MAX_PIXEL_MAP_TABLE", 0x0D34 },
	{ "GL_MAX_ATTRIB_STACK_DEPTH", 0x0D35 },
	{ "GL_MAX_MODELVIEW_STACK_DEPTH", 0x0D36 },
	{ "GL_MAX_NAME_STACK_DEPTH", 0x0D37 },
	{ "GL_MAX_PROJECTION_STACK_DEPTH", 0x0D38 },
	{ "GL_MAX_TEXTURE_STACK_DEPTH", 0x0D39 },
	{ "GL_MAX_VIEWPORT_DIMS", 0x0D3A },
	{ "GL_MAX_CLIENT_ATTRIB_STACK_DEPTH", 0x0D3B },
	{ "GL_SUBPIXEL_BITS", 0x0D50 },
	{ "GL_INDEX_BITS", 0x0D51 },
	{ "GL_RED_BITS", 0x0D52 },
	{ "GL_GREEN_BITS", 0x0D53 },
	{ "GL_BLUE_BITS", 0x0D54 },
	{ "GL_ALPHA_BITS", 0x0D55 },
	{ "GL_DEPTH_BITS", 0x0D56 },
	{ "GL_STENCIL_BITS", 0x0D57 },
	{ "GL_ACCUM_RED_BITS", 0x0D58 },
	{ "GL_ACCUM_GREEN_BITS", 0x0D59 },
	{ "GL_ACCUM_BLUE_BITS", 0x0D5A },
	{ "GL_ACCUM_ALPHA_BITS", 0x0D5B },
	{ "GL_NAME_STACK_DEPTH", 0x0D70 },
	{ "GL_AUTO_NORMAL", 0x0D80 },
	{ "GL_MAP1_COLOR_4", 0x0D90 },
	{ "GL_MAP1_INDEX", 0x0D91 },
	{ "GL_MAP1_NORMAL", 0x0D92 },
	{ "GL_MAP1_TEXTURE_COORD_1", 0x0D93 },
	{ "GL_MAP1_TEXTURE_COORD_2", 0x0D94 },
	{ "GL_MAP1_TEXTURE_COORD_3", 0x0D95 },
	{ "GL_MAP1_TEXTURE_COORD_4", 0x0D96 },
	{ "GL_MAP1_VERTEX_3", 0x0D97 },
	{ "GL_MAP1_VERTEX_4", 0x0D98 },
	{ "GL_MAP2_COLOR_4", 0x0DB0 },
	{ "GL_MAP2_INDEX", 0x0DB1 },
	{ "GL_MAP2_NORMAL", 0x0DB2 },
	{ "GL_MAP2_TEXTURE_COORD_1", 0x0DB3 },
	{ "GL_MAP2_TEXTURE_COORD_2", 0x0DB4 },
	{ "GL_MAP2_TEXTURE_COORD_3", 0x0DB5 },
	{ "GL_MAP2_TEXTURE_COORD_4", 0x0DB6 },
	{ "GL_MAP2_VERTEX_3", 0x0DB7 },
	{ "GL_MAP2_VERTEX_4", 0x0DB8 },
	{ "GL_MAP1_GRID_DOMAIN", 0x0DD0 },
	{ "GL_MAP1_GRID_SEGMENTS", 0x0DD1 },
	{ "GL_MAP2_GRID_DOMAIN", 0x0DD2 },
	{ "GL_MAP2_GRID_SEGMENTS", 0x0DD3 },
	{ "GL_TEXTURE_1D", 0x0DE0 },
	{ "GL_TEXTURE_2D", 0x0DE1 },
	{ "GL_FEEDBACK_BUFFER_POINTER", 0x0DF0 },
	{ "GL_FEEDBACK_BUFFER_SIZE", 0x0DF1 },
	{ "GL_FEEDBACK_BUFFER_TYPE", 0x0DF2 },
	{ "GL_SELECTION_BUFFER_POINTER", 0x0DF3 },
	{ "GL_SELECTION_BUFFER_SIZE", 0x0DF4 },
	{ "GL_TEXTURE_WIDTH", 0x1000 },
	{ "GL_TEXTURE_HEIGHT", 0x1001 },
	{ "GL_TEXTURE_INTERNAL_FORMAT", 0x1003 },
	{ "GL_TEXTURE_BORDER_COLOR", 0x1004 },
	{ "GL_TEXTURE_BORDER", 0x1005 },
	{ "GL_DONT_CARE", 0x1100 },
	{ "GL_FASTEST", 0x1101 },
	{ "GL_NICEST", 0x1102 },
	{ "GL_AMBIENT", 0x1200 },
	{ "GL_DIFFUSE", 0x1201 },
	{ "GL_SPECULAR", 0x1202 },
	{ "GL_POSITION", 0x1203 },
	{ "GL_SPOT_DIRECTION", 0x1204 },
	{ "GL_SPOT_EXPONENT", 0x1205 },
	{ "GL_SPOT_CUTOFF", 0x1206 },
	{ "GL_CONSTANT_ATTENUATION", 0x1207 },
	{ "GL_LINEAR_ATTENUATION", 0x1208 },
	{ "GL_QUADRATIC_ATTENUATION", 0x1209 },
	{ "GL_COMPILE", 0x1300 },
	{ "GL_COMPILE_AND_EXECUTE", 0x1301 },
	{ "GL_BYTE", 0x1400 },
	{ "GL_UNSIGNED_BYTE", 0x1401 },
	{ "GL_SHORT", 0x1402 },
	{ "GL_UNSIGNED_SHORT", 0x1403 },
	{ "GL_INT", 0x1404 },
	{ "GL_UNSIGNED_INT", 0x1405 },
	{ "GL_FLOAT", 0x1406 },
	{ "GL_2_BYTES", 0x1407 },
	{ "GL_3_BYTES", 0x1408 },
	{ "GL_4_BYTES", 0x1409 },
	{ "GL_DOUBLE", 0x140A },
	{ "GL_CLEAR", 0x1500 },
	{ "GL_AND", 0x1501 },
	{ "GL_AND_REVERSE", 0x1502 },
	{ "GL_COPY", 0x1503 },
	{ "GL_AND_INVERTED", 0x1504 },
	{ "GL_NOOP", 0x1505 },
	{ "GL_XOR", 0x1506 },
	{ "GL_OR", 0x1507 },
	{ "GL_NOR", 0x1508 },
	{ "GL_EQUIV", 0x1509 },
	{ "GL_INVERT", 0x150A },
	{ "GL_OR_REVERSE", 0x150B },
	{ "GL_COPY_INVERTED", 0x150C },
	{ "GL_OR_INVERTED", 0x150D },
	{ "GL_NAND", 0x150E },
	{ "GL_SET", 0x150F },
	{ "GL_EMISSION", 0x1600 },
	{ "GL_SHININESS", 0x1601 },
	{ "GL_AMBIENT_AND_DIFFUSE", 0x1602 },
	{ "GL_COLOR_INDEXES", 0x1603 },
	{ "GL_MODELVIEW", 0x1700 },
	{ "GL_PROJECTION", 0x1701 },
	{ "GL_TEXTURE", 0x1702 },
	{ "GL_COLOR", 0x1800 },
	{ "GL_DEPTH", 0x1801 },
	{ "GL_STENCIL", 0x1802 },
	{ "GL_COLOR_INDEX", 0x1900 },
	{ "GL_STENCIL_INDEX", 0x1901 },
	{ "GL_DEPTH_COMPONENT", 0x1902 },
	{ "GL_RED", 0x1903 },
	{ "GL_GREEN", 0x1904 },
	{ "GL_BLUE", 0x1905 },
	{ "GL_ALPHA", 0x1906 },
	{ "GL_RGB", 0x1907 },
	{ "GL_RGBA", 0x1908 },
	{ "GL_LUMINANCE", 0x1909 },
	{ "GL_LUMINANCE_ALPHA", 0x190A },
	{ "GL_BITMAP", 0x1A00 },
	{ "GL_POINT", 0x1B00 },
	{ "GL_LINE", 0x1B01 },
	{ "GL_FILL", 0x1B02 },
	{ "GL_RENDER", 0x1C00 },
	{ "GL_FEEDBACK", 0x1C01 },
	{ "GL_SELECT", 0x1C02 },
	{ "GL_FLAT", 0x1D00 },
	{ "GL_SMOOTH", 0x1D01 },
	{ "GL_KEEP", 0x1E00 },
	{ "GL_REPLACE", 0x1E01 },
	{ "GL_INCR", 0x1E02 },
	{ "GL_DECR", 0x1E03 },
	{ "GL_VENDOR", 0x1F00 },
	{ "GL_RENDERER", 0x1F01 },
	{ "GL_VERSION", 0x1F02 },
	{ "GL_EXTENSIONS", 0x1F03 },
	{ "GL_S", 0x2000 },
	{ "GL_T", 0x2001 },
	{ "GL_R", 0x2002 },
	{ "GL_Q", 0x2003 },
	{ "GL_MODULATE", 0x2100 },
	{ "GL_DECAL", 0x2101 },
	{ "GL_TEXTURE_ENV_MODE", 0x2200 },
	{ "GL_TEXTURE_ENV_COLOR", 0x2201 },
	{ "GL_TEXTURE_ENV", 0x2300 },
	{ "GL_EYE_LINEAR", 0x2400 },
	{ "GL_OBJECT_LINEAR", 0x2401 },
	{ "GL_SPHERE_MAP", 0x2402 },
	{ "GL_TEXTURE_GEN_MODE", 0x2500 },
	{ "GL_OBJECT_PLANE", 0x2501 },
	{ "GL_EYE_PLANE", 0x2502 },
	{ "GL_NEAREST", 0x2600 },
	{ "GL_LINEAR", 0x2601 },
	{ "GL_NEAREST_MIPMAP_NEAREST", 0x2700 },
	{ "GL_LINEAR_MIPMAP_NEAREST", 0x2701 },
	{ "GL_NEAREST_MIPMAP_LINEAR", 0x2702 },
	{ "GL_LINEAR_MIPMAP_LINEAR", 0x2703 },
	{ "GL_TEXTURE_MAG_FILTER", 0x2800 },
	{ "GL_TEXTURE_MIN_FILTER", 0x2801 },
	{ "GL_TEXTURE_WRAP_S", 0x2802 },
	{ "GL_TEXTURE_WRAP_T", 0x2803 },
	{ "GL_CLAMP", 0x2900 },
	{ "GL_REPEAT", 0x2901 },
	{ "GL_POLYGON_OFFSET_UNITS", 0x2A00 },
	{ "GL_POLYGON_OFFSET_POINT", 0x2A01 },
	{ "GL_POLYGON_OFFSET_LINE", 0x2A02 },
	{ "GL_R3_G3_B2", 0x2A10 },
	{ "GL_V2F", 0x2A20 },
	{ "GL_V3F", 0x2A21 },
	{ "GL_C4UB_V2F", 0x2A22 },
	{ "GL_C4UB_V3F", 0x2A23 },
	{ "GL_C3F_V3F", 0x2A24 },
	{ "GL_N3F_V3F", 0x2A25 },
	{ "GL_C4F_N3F_V3F", 0x2A26 },
	{ "GL_T2F_V3F", 0x2A27 },
	{ "GL_T4F_V4F", 0x2A28 },
	{ "GL_T2F_C4UB_V3F", 0x2A29 },
	{ "GL_T2F_C3F_V3F", 0x2A2A },
	{ "GL_T2F_N3F_V3F", 0x2A2B },
	{ "GL_T2F_C4F_N3F_V3F", 0x2A2C },
	{ "GL_T4F_C4F_N3F_V4F", 0x2A2D },
	{ "GL_CLIP_PLANE0", 0x3000 },
	{ "GL_CLIP_PLANE1", 0x3001 },
	{ "GL_CLIP_PLANE2", 0x3002 },
	{ "GL_CLIP_PLANE3", 0x3003 },
	{ "GL_CLIP_PLANE4", 0x3004 },
	{ "GL_CLIP_PLANE5", 0x3005 },
	{ "GL_LIGHT0", 0x4000 },
	{ "GL_LIGHT1", 0x4001 },
	{ "GL_LIGHT2", 0x4002 },
	{ "GL_LIGHT3", 0x4003 },
	{ "GL_LIGHT4", 0x4004 },
	{ "GL_LIGHT5", 0x4005 },
	{ "GL_LIGHT6", 0x4006 },
	{ "GL_LIGHT7", 0x4007 },
	{ "GL_POLYGON_OFFSET_FILL", 0x8037 },
	{ "GL_POLYGON_OFFSET_FACTOR", 0x8038 },
	{ "GL_ALPHA4", 0x803B },
	{ "GL_ALPHA8", 0x803C },
	{ "GL_ALPHA12", 0x803D },
	{ "GL_ALPHA16", 0x803E },
	{ "GL_LUMINANCE4", 0x803F },
	{ "GL_LUMINANCE8", 0x8040 },
	{ "GL_LUMINANCE12", 0x8041 },
	{ "GL_LUMINANCE16", 0x8042 },
	{ "GL_LUMINANCE4_ALPHA4", 0x8043 },
	{ "GL_LUMINANCE6_ALPHA2", 0x8044 },
	{ "GL_LUMINANCE8_ALPHA8", 0x8045 },
	{ "GL_LUMINANCE12_ALPHA4", 0x8046 },
	{ "GL_LUMINANCE12_ALPHA12", 0x8047 },
	{ "GL_LUMINANCE16_ALPHA16", 0x8048 },
	{ "GL_INTENSITY", 0x8049 },
	{ "GL_INTENSITY4", 0x804A },
	{ "GL_INTENSITY8", 0x804B },
	{ "GL_INTENSITY12", 0x804C },
	{ "GL_INTENSITY16", 0x804D },
	{ "GL_RGB4", 0x804F },
	{ "GL_RGB5", 0x8050 },
	{ "GL_RGB8", 0x8051 },
	{ "GL_RGB10", 0x8052 },
	{ "GL_RGB12", 0x8053 },
	{ "GL_RGB16", 0x8054 },
	{ "GL_RGBA2", 0x8055 },
	{ "GL_RGBA4", 0x8056 },
	{ "GL_RGB5_A1", 0x8057 },
	{ "GL_RGBA8", 0x8058 },
	{ "GL_RGB10_A2", 0x8059 },
	{ "GL_RGBA12", 0x805A },
	{ "GL_RGBA16", 0x805B },
	{ "GL_TEXTURE_RED_SIZE", 0x805C },
	{ "GL_TEXTURE_GREEN_SIZE", 0x805D },
	{ "GL_TEXTURE_BLUE_SIZE", 0x805E },
	{ "GL_TEXTURE_ALPHA_SIZE", 0x805F },
	{ "GL_TEXTURE_LUMINANCE_SIZE", 0x8060 },
	{ "GL_TEXTURE_INTENSITY_SIZE", 0x8061 },
	{ "GL_PROXY_TEXTURE_1D", 0x8063 },
	{ "GL_PROXY_TEXTURE_2D", 0x8064 },
	{ "GL_TEXTURE_PRIORITY", 0x8066 },
	{ "GL_TEXTURE_RESIDENT", 0x8067 },
	{ "GL_TEXTURE_BINDING_1D", 0x8068 },
	{ "GL_TEXTURE_BINDING_2D", 0x8069 },
	{ "GL_VERTEX_ARRAY", 0x8074 },
	{ "GL_NORMAL_ARRAY", 0x8075 },
	{ "GL_COLOR_ARRAY", 0x8076 },
	{ "GL_INDEX_ARRAY", 0x8077 },
	{ "GL_TEXTURE_COORD_ARRAY", 0x8078 },
	{ "GL_EDGE_FLAG_ARRAY", 0x8079 },
	{ "GL_VERTEX_ARRAY_SIZE", 0x807A },
	{ "GL_VERTEX_ARRAY_TYPE", 0x807B },
	{ "GL_VERTEX_ARRAY_STRIDE", 0x807C },
	{ "GL_NORMAL_ARRAY_TYPE", 0x807E },
	{ "GL_NORMAL_ARRAY_STRIDE", 0x807F },
	{ "GL_COLOR_ARRAY_SIZE", 0x8081 },
	{ "GL_COLOR_ARRAY_TYPE", 0x8082 },
	{ "GL_COLOR_ARRAY_STRIDE", 0x8083 },
	{ "GL_INDEX_ARRAY_TYPE", 0x8085 },
	{ "GL_INDEX_ARRAY_STRIDE", 0x8086 },
	{ "GL_TEXTURE_COORD_ARRAY_SIZE", 0x8088 },
	{ "GL_TEXTURE_COORD_ARRAY_TYPE", 0x8089 },
	{ "GL_TEXTURE_COORD_ARRAY_STRIDE", 0x808A },
	{ "GL_EDGE_FLAG_ARRAY_STRIDE", 0x808C },
	{ "GL_VERTEX_ARRAY_POINTER", 0x808E },
	{ "GL_NORMAL_ARRAY_POINTER", 0x808F },
	{ "GL_COLOR_ARRAY_POINTER", 0x8090 },
	{ "GL_INDEX_ARRAY_POINTER", 0x8091 },
	{ "GL_TEXTURE_COORD_ARRAY_POINTER", 0x8092 },
	{ "GL_EDGE_FLAG_ARRAY_POINTER", 0x8093 },
	{ "GL_COLOR_TABLE_FORMAT_EXT", 0x80D8 },
	{ "GL_COLOR_TABLE_WIDTH_EXT", 0x80D9 },
	{ "GL_COLOR_TABLE_RED_SIZE_EXT", 0x80DA },
	{ "GL_COLOR_TABLE_GREEN_SIZE_EXT", 0x80DB },
	{ "GL_COLOR_TABLE_BLUE_SIZE_EXT", 0x80DC },
	{ "GL_COLOR_TABLE_ALPHA_SIZE_EXT", 0x80DD },
	{ "GL_COLOR_TABLE_LUMINANCE_SIZE_EXT", 0x80DE },
	{ "GL_COLOR_TABLE_INTENSITY_SIZE_EXT", 0x80DF },
	{ "GL_COLOR_INDEX1_EXT", 0x80E2 },
	{ "GL_COLOR_INDEX2_EXT", 0x80E3 },
	{ "GL_COLOR_INDEX4_EXT", 0x80E4 },
	{ "GL_COLOR_INDEX8_EXT", 0x80E5 },
	{ "GL_COLOR_INDEX12_EXT", 0x80E6 },
	{ "GL_COLOR_INDEX16_EXT", 0x80E7 },

	/* EXT_bgra */
	{ "GL_BGR_EXT", 0x80E0 },
	{ "GL_BGRA_EXT", 0x80E1 },

	/* GL_EXT_point_parameters */
	{ "GL_POINT_SIZE_MIN_EXT", 0x8126 },
	{ "GL_POINT_SIZE_MAX_EXT", 0x8127 },
	{ "GL_POINT_FADE_THRESHOLD_SIZE_EXT", 0x8128 },
	{ "GL_DISTANCE_ATTENUATION_EXT", 0x8129 },

	/* GL_ARB_multitexture */
	{ "GL_ACTIVE_TEXTURE_ARB", 0x84E0 },
	{ "GL_CLIENT_ACTIVE_TEXTURE_ARB", 0x84E1 },
	{ "GL_MAX_TEXTURES_UNITS_ARB", 0x84E2 },
	{ "GL_TEXTURE0_ARB", 0x84C0 },
	{ "GL_TEXTURE1_ARB", 0x84C1 },
	{ "GL_TEXTURE2_ARB", 0x84C2 },
	{ "GL_TEXTURE3_ARB", 0x84C3 },
	{ "GL_TEXTURE4_ARB", 0x84C4 },
	{ "GL_TEXTURE5_ARB", 0x84C5 },
	{ "GL_TEXTURE6_ARB", 0x84C6 },
	{ "GL_TEXTURE7_ARB", 0x84C7 },
	{ "GL_TEXTURE8_ARB", 0x84C8 },
	{ "GL_TEXTURE9_ARB", 0x84C9 },
	{ "GL_TEXTURE10_ARB", 0x84CA },
	{ "GL_TEXTURE11_ARB", 0x84CB },
	{ "GL_TEXTURE12_ARB", 0x84CC },
	{ "GL_TEXTURE13_ARB", 0x84CD },
	{ "GL_TEXTURE14_ARB", 0x84CE },
	{ "GL_TEXTURE15_ARB", 0x84CF },
	{ "GL_TEXTURE16_ARB", 0x84D0 },
	{ "GL_TEXTURE17_ARB", 0x84D1 },
	{ "GL_TEXTURE18_ARB", 0x84D2 },
	{ "GL_TEXTURE19_ARB", 0x84D3 },
	{ "GL_TEXTURE20_ARB", 0x84D4 },
	{ "GL_TEXTURE21_ARB", 0x84D5 },
	{ "GL_TEXTURE22_ARB", 0x84D6 },
	{ "GL_TEXTURE23_ARB", 0x84D7 },
	{ "GL_TEXTURE24_ARB", 0x84D8 },
	{ "GL_TEXTURE25_ARB", 0x84D9 },
	{ "GL_TEXTURE26_ARB", 0x84DA },
	{ "GL_TEXTURE27_ARB", 0x84DB },
	{ "GL_TEXTURE28_ARB", 0x84DC },
	{ "GL_TEXTURE29_ARB", 0x84DD },
	{ "GL_TEXTURE30_ARB", 0x84DE },
	{ "GL_TEXTURE31_ARB", 0x84DF },

	/* Packed pixels */
	{ "GL_UNSIGNED_BYTE_3_3_2", 0x8032 },
	{ "GL_UNSIGNED_BYTE_2_3_3_REV", 0x8362 },
	{ "GL_UNSIGNED_SHORT_5_6_5", 0x8363 },
	{ "GL_UNSIGNED_SHORT_5_6_5_REV", 0x8364 },
	{ "GL_UNSIGNED_SHORT_4_4_4_4", 0x8033 },
	{ "GL_UNSIGNED_SHORT_4_4_4_4_REV", 0x8365 },
	{ "GL_UNSIGNED_SHORT_5_5_5_1", 0x8034 },
	{ "GL_UNSIGNED_SHORT_1_5_5_5_REV", 0x8366 },
	{ "GL_UNSIGNED_INT_8_8_8_8", 0x8035 },
	{ "GL_UNSIGNED_INT_8_8_8_8_REV", 0x8367 },
	{ "GL_UNSIGNED_INT_10_10_10_2", 0x8036 },
	{ "GL_UNSIGNED_INT_2_10_10_10_REV", 0x8368 },

	/* EXT_texture_env_combine */
	{ "GL_COMBINE_EXT", 0x8570 },
	{ "GL_COMBINE_RGB_EXT", 0x8571 },
	{ "GL_COMBINE_ALPHA_EXT", 0x8572 },
	{ "GL_RGB_SCALE_EXT", 0x8573 },
	{ "GL_ADD_SIGNED_EXT", 0x8574 },
	{ "GL_INTERPOLATE_EXT", 0x8575 },
	{ "GL_CONSTANT_EXT", 0x8576 },
	{ "GL_PRIMARY_COLOR_EXT", 0x8577 },
	{ "GL_PREVIOUS_EXT", 0x8578 },
	{ "GL_SOURCE0_RGB_EXT", 0x8580 },
	{ "GL_SOURCE1_RGB_EXT", 0x8581 },
	{ "GL_SOURCE2_RGB_EXT", 0x8582 },
	{ "GL_SOURCE0_ALPHA_EXT", 0x8588 },
	{ "GL_SOURCE1_ALPHA_EXT", 0x8589 },
	{ "GL_SOURCE2_ALPHA_EXT", 0x858A },
	{ "GL_OPERAND0_RGB_EXT", 0x8590 },
	{ "GL_OPERAND1_RGB_EXT", 0x8591 },
	{ "GL_OPERAND2_RGB_EXT", 0x8592 },
	{ "GL_OPERAND0_ALPHA_EXT", 0x8598 },
	{ "GL_OPERAND1_ALPHA_EXT", 0x8599 },
	{ "GL_OPERAND2_ALPHA_EXT", 0x859A },

	/* EXT_abgr */
	{ "GL_ABGR_EXT", 0x8000 },

	/* EXT_clip_volume_hint */
	{ "GL_CLIP_VOLUME_CLIPPING_HINT_EXT", 0x80F0 },

	/* EXT_compiled_vertex_array */
	{ "GL_ARRAY_ELEMENT_LOCK_FIRST_EXT", 0x81A8 },
	{ "GL_ARRAY_ELEMENT_LOCK_COUNT_EXT", 0x81A9 },

	/* EXT_cull_vertex */
	{ "GL_CULL_VERTEX_EXT", 0x81AA },
	{ "GL_CULL_VERTEX_EYE_POSITION_EXT", 0x81AB },
	{ "GL_CULL_VERTEX_OBJECT_POSITION_EXT", 0x81AC },

	/* EXT_stencil_wrap */
	{ "GL_INCR_WRAP_EXT", 0x8507 },
	{ "GL_DECR_WRAP_EXT", 0x8508 },

	/* NV_texture_env_combine4 */
	{ "GL_COMBINE4_NV", 0x8503 },
	{ "GL_SOURCE3_RGB_NV", 0x8583 },
	{ "GL_SOURCE3_ALPHA_NV", 0x858B },
	{ "GL_OPERAND3_RGB_NV", 0x8593 },
	{ "GL_OPERAND3_ALPHA_NV", 0x859B },

	/* ARB_texture_cube_map */
	{ "GL_NORMAL_MAP_ARB", 0x8511 },
	{ "GL_REFLECTION_MAP_ARB", 0x8512 },
	{ "GL_TEXTURE_CUBE_MAP_ARB", 0x8513 },
	{ "GL_TEXTURE_BINDING_CUBE_MAP_ARB", 0x8514 },
	{ "GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB", 0x8515 },
	{ "GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB", 0x8516 },
	{ "GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB", 0x8517 },
	{ "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB", 0x8518 },
	{ "GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB", 0x8519 },
	{ "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB", 0x851A },
	{ "GL_PROXY_TEXTURE_CUBE_MAP_ARB", 0x851B },
	{ "GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB", 0x851C },

	/* EXT_vertex_weighting */
	{ "GL_VERTEX_WEIGHTING_EXT", 0x8509 },
	{ "GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT", 0x8510 },
	{ "GL_MODELVIEW1_EXT", 0x850A },
	{ "GL_CURRENT_VERTEX_WEIGHT_EXT", 0x850B },
	{ "GL_VERTEX_WEIGHT_ARRAY_EXT", 0x850C },
	{ "GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT", 0x850D },
	{ "GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT", 0x850E },
	{ "GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT", 0x850F },
	{ "GL_MODELVIEW1_STACK_DEPTH_EXT", 0x8502 },

	/* GL_S3_s3tc */
	{ "GL_RGB_S3TC", 0x83A0 },
	{ "GL_RGB4_S3TC", 0x83A1 },
	{ "GL_RGBA_S3TC", 0x83A2 },
	{ "GL_RGBA4_S3TC", 0x83A3 },

	/* ARB_transpose_matrix */
	{ "GL_TRANSPOSE_MODELVIEW_MATRIX_ARB", 0x84E3 },
	{ "GL_TRANSPOSE_PROJECTION_MATRIX_ARB", 0x84E4 },
	{ "GL_TRANSPOSE_TEXTURE_MATRIX_ARB", 0x84E5 },
	{ "GL_TRANSPOSE_COLOR_MATRIX_ARB", 0x84E6 },

	/* ARB_multisample */
	{ "GL_WGL_SAMPLE_BUFFERS_ARB", 0x2041 },
	{ "GL_WGL_SAMPLES_ARB", 0x2042 },
	{ "GL_MULTISAMPLE_ARB", 0x809D },
	{ "GL_SAMPLE_ALPHA_TO_COVERAGE_ARB", 0x809E },
	{ "GL_SAMPLE_ALPHA_TO_ONE_ARB", 0x809F },
	{ "GL_SAMPLE_COVERAGE_ARB", 0x80A0 },
	{ "GL_SAMPLE_BUFFERS_ARB", 0x80A8 },
	{ "GL_SAMPLES_ARB", 0x80A9 },
	{ "GL_SAMPLE_COVERAGE_VALUE_ARB", 0x80AA },
	{ "GL_SAMPLE_COVERAGE_INVERT_ARB", 0x80AB },

	/* WGL_ARB_make_current_read */
	{ "GL_ERROR_INVALID_PIXEL_TYPE_ARB", 0x2043 },
	{ "ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB", 0x2054 }
};

/* Names of OpenGL functions */
char			*GLN[] =
{
	"glAccum",
	"glAlphaFunc",
	"glAreTexturesResident",
	"glArrayElement",
	"glBegin",
	"glBindTexture",
	"glBitmap",
	"glBlendFunc",
	"glCallList",
	"glCallLists",
	"glClear",
	"glClearAccum",
	"glClearColor",
	"glClearDepth",
	"glClearIndex",
	"glClearStencil",
	"glClipPlane",
	"glColor3b",
	"glColor3bv",
	"glColor3d",
	"glColor3dv",
	"glColor3f",
	"glColor3fv",
	"glColor3i",
	"glColor3iv",
	"glColor3s",
	"glColor3sv",
	"glColor3ub",
	"glColor3ubv",
	"glColor3ui",
	"glColor3uiv",
	"glColor3us",
	"glColor3usv",
	"glColor4b",
	"glColor4bv",
	"glColor4d",
	"glColor4dv",
	"glColor4f",
	"glColor4fv",
	"glColor4i",
	"glColor4iv",
	"glColor4s",
	"glColor4sv",
	"glColor4ub",
	"glColor4ubv",
	"glColor4ui",
	"glColor4uiv",
	"glColor4us",
	"glColor4usv",
	"glColorMask",
	"glColorMaterial",
	"glColorPointer",
	"glCopyPixels",
	"glCopyTexImage1D",
	"glCopyTexImage2D",
	"glCopyTexSubImage1D",
	"glCopyTexSubImage2D",
	"glCullFace",
	"glDeleteLists",
	"glDeleteTextures",
	"glDepthFunc",
	"glDepthMask",
	"glDepthRange",
	"glDisable",
	"glDisableClientState",
	"glDrawArrays",
	"glDrawBuffer",
	"glDrawElements",
	"glDrawPixels",
	"glEdgeFlag",
	"glEdgeFlagPointer",
	"glEdgeFlagv",
	"glEnable",
	"glEnableClientState",
	"glEnd",
	"glEndList",
	"glEvalCoord1d",
	"glEvalCoord1dv",
	"glEvalCoord1f",
	"glEvalCoord1fv",
	"glEvalCoord2d",
	"glEvalCoord2dv",
	"glEvalCoord2f",
	"glEvalCoord2fv",
	"glEvalMesh1",
	"glEvalMesh2",
	"glEvalPoint1",
	"glEvalPoint2",
	"glFeedbackBuffer",
	"glFinish",
	"glFlush",
	"glFogf",
	"glFogfv",
	"glFogi",
	"glFogiv",
	"glFrontFace",
	"glFrustum",
	"glGenLists",
	"glGenTextures",
	"glGetBooleanv",
	"glGetClipPlane",
	"glGetDoublev",
	"glGetError",
	"glGetFloatv",
	"glGetIntegerv",
	"glGetLightfv",
	"glGetLightiv",
	"glGetMapdv",
	"glGetMapfv",
	"glGetMapiv",
	"glGetMaterialfv",
	"glGetMaterialiv",
	"glGetPixelMapfv",
	"glGetPixelMapuiv",
	"glGetPixelMapusv",
	"glGetPointerv",
	"glGetPolygonStipple",
	"glGetString",
	"glGetTexEnvfv",
	"glGetTexEnviv",
	"glGetTexGendv",
	"glGetTexGenfv",
	"glGetTexGeniv",
	"glGetTexImage",
	"glGetTexLevelParameterfv",
	"glGetTexLevelParameteriv",
	"glGetTexParameterfv",
	"glGetTexParameteriv",
	"glHint",
	"glIndexMask",
	"glIndexPointer",
	"glIndexd",
	"glIndexdv",
	"glIndexf",
	"glIndexfv",
	"glIndexi",
	"glIndexiv",
	"glIndexs",
	"glIndexsv",
	"glIndexub",
	"glIndexubv",
	"glInitNames",
	"glInterleavedArrays",
	"glIsEnabled",
	"glIsList",
	"glIsTexture",
	"glLightModelf",
	"glLightModelfv",
	"glLightModeli",
	"glLightModeliv",
	"glLightf",
	"glLightfv",
	"glLighti",
	"glLightiv",
	"glLineStipple",
	"glLineWidth",
	"glListBase",
	"glLoadIdentity",
	"glLoadMatrixd",
	"glLoadMatrixf",
	"glLoadName",
	"glLogicOp",
	"glMap1d",
	"glMap1f",
	"glMap2d",
	"glMap2f",
	"glMapGrid1d",
	"glMapGrid1f",
	"glMapGrid2d",
	"glMapGrid2f",
	"glMaterialf",
	"glMaterialfv",
	"glMateriali",
	"glMaterialiv",
	"glMatrixMode",
	"glMultMatrixd",
	"glMultMatrixf",
	"glNewList",
	"glNormal3b",
	"glNormal3bv",
	"glNormal3d",
	"glNormal3dv",
	"glNormal3f",
	"glNormal3fv",
	"glNormal3i",
	"glNormal3iv",
	"glNormal3s",
	"glNormal3sv",
	"glNormalPointer",
	"glOrtho",
	"glPassThrough",
	"glPixelMapfv",
	"glPixelMapuiv",
	"glPixelMapusv",
	"glPixelStoref",
	"glPixelStorei",
	"glPixelTransferf",
	"glPixelTransferi",
	"glPixelZoom",
	"glPointSize",
	"glPolygonMode",
	"glPolygonOffset",
	"glPolygonStipple",
	"glPopAttrib",
	"glPopClientAttrib",
	"glPopMatrix",
	"glPopName",
	"glPrioritizeTextures",
	"glPushAttrib",
	"glPushClientAttrib",
	"glPushMatrix",
	"glPushName",
	"glRasterPos2d",
	"glRasterPos2dv",
	"glRasterPos2f",
	"glRasterPos2fv",
	"glRasterPos2i",
	"glRasterPos2iv",
	"glRasterPos2s",
	"glRasterPos2sv",
	"glRasterPos3d",
	"glRasterPos3dv",
	"glRasterPos3f",
	"glRasterPos3fv",
	"glRasterPos3i",
	"glRasterPos3iv",
	"glRasterPos3s",
	"glRasterPos3sv",
	"glRasterPos4d",
	"glRasterPos4dv",
	"glRasterPos4f",
	"glRasterPos4fv",
	"glRasterPos4i",
	"glRasterPos4iv",
	"glRasterPos4s",
	"glRasterPos4sv",
	"glReadBuffer",
	"glReadPixels",
	"glRectd",
	"glRectdv",
	"glRectf",
	"glRectfv",
	"glRecti",
	"glRectiv",
	"glRects",
	"glRectsv",
	"glRenderMode",
	"glRotated",
	"glRotatef",
	"glScaled",
	"glScalef",
	"glScissor",
	"glSelectBuffer",
	"glShadeModel",
	"glStencilFunc",
	"glStencilMask",
	"glStencilOp",
	"glTexCoord1d",
	"glTexCoord1dv",
	"glTexCoord1f",
	"glTexCoord1fv",
	"glTexCoord1i",
	"glTexCoord1iv",
	"glTexCoord1s",
	"glTexCoord1sv",
	"glTexCoord2d",
	"glTexCoord2dv",
	"glTexCoord2f",
	"glTexCoord2fv",
	"glTexCoord2i",
	"glTexCoord2iv",
	"glTexCoord2s",
	"glTexCoord2sv",
	"glTexCoord3d",
	"glTexCoord3dv",
	"glTexCoord3f",
	"glTexCoord3fv",
	"glTexCoord3i",
	"glTexCoord3iv",
	"glTexCoord3s",
	"glTexCoord3sv",
	"glTexCoord4d",
	"glTexCoord4dv",
	"glTexCoord4f",
	"glTexCoord4fv",
	"glTexCoord4i",
	"glTexCoord4iv",
	"glTexCoord4s",
	"glTexCoord4sv",
	"glTexCoordPointer",
	"glTexEnvf",
	"glTexEnvfv",
	"glTexEnvi",
	"glTexEnviv",
	"glTexGend",
	"glTexGendv",
	"glTexGenf",
	"glTexGenfv",
	"glTexGeni",
	"glTexGeniv",
	"glTexImage1D",
	"glTexImage2D",
	"glTexParameterf",
	"glTexParameterfv",
	"glTexParameteri",
	"glTexParameteriv",
	"glTexSubImage1D",
	"glTexSubImage2D",
	"glTranslated",
	"glTranslatef",
	"glVertex2d",
	"glVertex2dv",
	"glVertex2f",
	"glVertex2fv",
	"glVertex2i",
	"glVertex2iv",
	"glVertex2s",
	"glVertex2sv",
	"glVertex3d",
	"glVertex3dv",
	"glVertex3f",
	"glVertex3fv",
	"glVertex3i",
	"glVertex3iv",
	"glVertex3s",
	"glVertex3sv",
	"glVertex4d",
	"glVertex4dv",
	"glVertex4f",
	"glVertex4fv",
	"glVertex4i",
	"glVertex4iv",
	"glVertex4s",
	"glVertex4sv",
	"glVertexPointer",
	"glViewport",
#if defined WIN32 || defined WIN64
	"wglChoosePixelFormat",
	"wglCopyContext",
	"wglCreateContext",
	"wglCreateLayerContext",
	"wglDeleteContext",
	"wglDescribeLayerPlane",
	"wglDescribePixelFormat",
	"wglGetCurrentContext",
	"wglGetCurrentDC",
	"wglGetDefaultProcAddress",
	"wglGetLayerPaletteEntries",
	"wglGetPixelFormat",
	"wglGetProcAddress",
	"wglMakeCurrent",
	"wglRealizeLayerPalette",
	"wglSetLayerPaletteEntries",
	"wglSetPixelFormat",
	"wglShareLists",
	"wglSwapBuffers",
	"wglSwapLayerBuffers",
	"wglUseFontBitmapsA",
	"wglUseFontBitmapsW",
	"wglUseFontOutlinesA",
	"wglUseFontOutlinesW"
#else
#endif
};
int				show_status[
ARY_CNT( GLN )
];
extern int		show_status_EXT[];
typedef enum
{
	NONE,
	CFG,
	KEY,
	IMP,
	EXC,
	OPT,
	OUTP
} section;


/*  gltraceInit() where the actual initialization take place */
GLboolean
gltraceInit ( void )
{
	int			i;
	char		INI_pathname[1024];
	char		OGL_pathname[1024];
	FILE		*INI;
	time_t		elapstime;
	struct tm	*adjtime;
	char		loctime[32];
	if ( needInit == GL_FALSE )
	{
		return ( GL_TRUE );
	}

	InitCallCount();
	for ( i = 0; i < ARY_CNT( show_status); i++ )
	{
		show_status[i] = SHOW_ALWAYS;
	}

	/* Build index for GLenum name lookup */
	for ( i = 0; i < ARY_CNT( GLenum_names); i++ )
	{
		GLenum_names[i] = NULL;
	}

	for ( i = 0; i < ARY_CNT( GLenum_list); i++ )
	{
		GLenum_names[GLenum_list[i].tag] = GLenum_list[i].name;
	}

	/* set the default OpenGL library path */
#if defined WIN32 || defined WIN64
	( void) GetSystemDirectory( OGL_pathname, 1024 );
	strcat( OGL_pathname, "\\opengl32.dll" );
#else
	strcpy( OGL_pathname, "libGL.so" );
#endif

	/* Parse .INI file */
	strcpy( INI_pathname, "OGLWrapper.INI" );							//Static
	INI = fopen( INI_pathname, "rt" );
	if ( INI != NULL )
	{
		char	line[256];
		int		keynumber = 0;
		section s = NONE;
		while ( fgets( line, sizeof(line) - 1, INI) != NULL )
		{
			if ( line[0] == ';' )
			{
				continue;
			}

			for ( i = strlen( line) - 1; i >= 0; i-- )
			{
				if ( isspace( (int) (line[i])) > 0 )
				{
					line[i] = (char) 0;
				}
				else
				{
					break;
				}
			}

			if ( i == -1 )
			{
				continue;
			}

			if ( strcmp( line, "[Config]") == 0 )
			{
				s = CFG;
				continue;
			}															//Static

			if ( strcmp( line, "[Keys]") == 0 )
			{
				s = KEY;
				continue;
			}

			if ( strcmp( line, "[Implementation]") == 0 )
			{
				s = IMP;
				continue;
			}

			if ( strcmp( line, "[Exclude]") == 0 )
			{
				s = EXC;
				continue;
			}

			if ( strcmp( line, "[Optional]") == 0 )
			{
				s = OPT;
				continue;
			}

			if ( strcmp( line, "[Output]") == 0 )
			{
				s = OUTP;
				continue;
			}

			switch ( s )
			{
				case CFG:
					if ( strcmp( "LogCalls", line) == 0 )
					{
						bLogEnable = GL_TRUE;
						setKeyState( GLT_ON_OFF, GL_TRUE );
						break;
					}

					if ( strcmp( "CountCalls", line) == 0 )
					{
						bCountCalls = GL_TRUE;
						break;
					}

					if ( strcmp( "Verbose", line) == 0 )
					{
						bVerbose = GL_TRUE;
						break;
					}

					if ( strcmp( "LogTime", line) == 0 )
					{
						bLogTime = GL_TRUE;
						break;
					}

					if ( strcmp( "FPS_White", line) == 0 )
					{
						fpsColor = GLT_WHITE;
						bFSPenable = GL_TRUE;
						break;
					}

					if ( strcmp( "FPS_Black", line) == 0 )
					{
						fpsColor = GLT_BLACK;
						bFSPenable = GL_TRUE;
						break;
					}

					if ( strcmp( "FPS_Yellow", line) == 0 )
					{
						fpsColor = GLT_YELLOW;
						bFSPenable = GL_TRUE;
						break;
					}

					if ( strcmp( "FPS_Gray", line) == 0 )
					{
						fpsColor = GLT_GRAY;
						bFSPenable = GL_TRUE;
						break;
					}

					if ( strcmp( "Time_Enabled_Yellow", line) == 0 )	//Static
					{
						timeColor = GLT_YELLOW;
						bTimeEnabled = GL_TRUE;
						break;
					}

					if ( strcmp( "Time_Enabled_Black", line) == 0 )		//Static
					{
						timeColor = GLT_BLACK;
						bTimeEnabled = GL_TRUE;
						break;
					}

					if ( strcmp( "Time_Enabled_White", line) == 0 )
					{													//Static
						timeColor = GLT_WHITE;
						bTimeEnabled = GL_TRUE;
						break;
					}

					if ( strcmp( "Time_Enabled_Test", line) == 0 )
					{													//Static
						timeColor = GLT_TEST;
						bTimeEnabled = GL_TRUE;
						break;
					}

					if ( strcmp( "Time_Enabled_Red", line) == 0 )
					{													//Static
						timeColor = GLT_RED;
						bTimeEnabled = GL_TRUE;
						break;
					}
					break;
				case KEY:
					{
						switch ( keynumber )
						{
							case 0:
								setKey( GLT_OPTION, line );
								break;
							case 1:
								setKey( GLT_ONEFRAME, line );
								break;
							case 2:
								setKey( GLT_FPS, line );
								break;
							case 3:
								setKey( GLT_ON_OFF, line );
								break;
							case 4:
								setKey( SV_TIME, line );
								break;
							case 5:
								setKey( SV_WALL, line );
								break;
							case 6:
								setKey( SV_WIRE, line );
								break;
							case 7:
								setKey( SV_TEST, line );
								break;
							default:
								break;
						}

						keynumber++;
					}
					break;
				case IMP:
					strncpy( OGL_pathname, line, 1023 );
					OGL_pathname[1023] = (char) 0;
					break;
				case OUTP:
					text_file = fopen( line, "w+t" );
					break;
				case EXC:
					for ( i = 0; i < ARY_CNT( GLN); i++ )
					{
						if ( strcmp( GLN[i], line) == 0 )
						{
							show_status[i] = SUPPRESS;
							break;
						}
					}
					{
						int index = GetEXTProcIndex( line );
						if ( index >= 0 )
						{
							show_status_EXT[index] = SUPPRESS;
							break;
						}
					}
					break;
				case OPT:
					for ( i = 0; i < ARY_CNT( GLN); i++ )
					{
						if ( strcmp( GLN[i], line) == 0 )
						{
							show_status[i] = SHOW_IF_SHIFT_DOWN;
							break;
						}
					}
					{
						int index = GetEXTProcIndex( line );
						if ( index >= 0 )
						{
							show_status_EXT[index] = SHOW_IF_SHIFT_DOWN;
							break;
						}
					}
					break;
				case NONE:
					break;
			}
		}
		(void) fclose( INI );
	}

	/* Load specified OpenGL provider asked for in the INI file */
	OpenGL_provider = dllLoad( OGL_pathname );
	if ( OpenGL_provider == NULL )
	{
		print( "Couldn't find OpenGL driver" );
		return ( GL_FALSE );
	}

	/* Map OpenGL calls to our jump table */
	for ( i = 0; i < ARY_CNT( GLN); i++ )
	{
		FUNCTION	OpenGL_proc = dllGetFunction( OpenGL_provider, GLN[i] );
		if ( OpenGL_proc == NULL )
		{
			print( "Couldn't GetProcAddress for %s\n", GLN[i] );
			return ( GL_FALSE );
		}
		( (FUNCTION *) (&GLV) )[i] = OpenGL_proc;
	}

	/* Indicate start of debug trace */
	(void) time( &elapstime );
	adjtime = localtime( &elapstime );
	strcpy( loctime, asctime( adjtime) );
	loctime[24] = (char) 0;
	print( "OpenGL provider: %s\n", OGL_pathname );
	print( "Start time: %s\n", loctime );
	ftime( &starttime );
	needInit = GL_FALSE;
	return ( GL_TRUE );
}

#if defined WIN32 || defined WIN64


/* */
BOOL WINAPI
DllMain ( /*@unused@*/ HINSTANCE hinstDLL, DWORD fdwReason, /*@unused@*/ LPVOID lpreserved )
{
	if ( fdwReason == DLL_PROCESS_DETACH )
	{
		if ( text_file != NULL )
		{
			PrintCallCounts();
			print( "Closing output file\n" );
			(void) fclose( text_file );
			text_file = NULL;
		}

		dllUnload( OpenGL_provider );
	}

	return ( BOOL ) TRUE;
}

#else

/* */
void
_init ( void )
{
	gltraceInit();
}


/* */
void
_fini ( void )
{
	if ( text_file != NULL )
	{
		PrintCallCounts();
		print( "Closing output file\n" );
		fclose( text_file );
		text_file = NULL;
	}

	dllUnload( OpenGL_provider );
}
#endif /* WIN32 */
