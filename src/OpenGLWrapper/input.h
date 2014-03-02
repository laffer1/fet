#ifndef INPUT_H
#define INPUT_H
#ifdef __cplusplus
extern "C"
{
#endif
#define GLT_OPTION		0
#define GLT_ONEFRAME	1
#define GLT_FPS			2
#define GLT_ON_OFF		3

	/*********************************/

	/* Hack Defines for Toggling :-) */

	/*********************************/
#define SV_TIME 4	// Time Hack
#define SV_WIRE 5	// Wire Frame
#define SV_WALL 6	// Wall Hack
#define SV_TEST 7	// For Testing
	void		updateKeyState ( void );
	GLboolean	setKey ( int function, char *key );
	GLboolean	getKeyState ( int function );
	void		setKeyState ( int function, GLboolean state );
#ifdef __cplusplus
}
#endif
#endif /* INPUT_H */
