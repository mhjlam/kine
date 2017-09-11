/*
 * STICKLER
 *
 * Author	Maurits Lam
 * Date		September 2017
 *
 * Description
 * 	Simple 2D and 3D kinematics simulation.
 *	Still needs a lot of improvements.
 *
 *
 *	To switch between 2D/3D mode, use the pop-up menu (right mouse button).
 *	Any changes made to each mode are persistent when switching.
 *
 *	Use the left and right arrow keys to change the active joint.
 *	Use the up and down arrow keys to change the active joint's angle of rotation.
 *
 *	In 2D mode, attach nodes (points) to the nearest joint via the context menu.
 *  
 *	In 3D mode, click-drag the mouse in the lower-left corner to rotate the view,
 *  and use the x, y and z keys to change the active axis of rotation.
*/

#include "stickler.h"

int main(int argc, char* argv[])
{
	stickler stickler;
	stickler.init(argc, argv);
	stickler.run(1280, 720);
	return 0;
}
