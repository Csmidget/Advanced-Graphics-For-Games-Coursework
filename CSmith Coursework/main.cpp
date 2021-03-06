#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Cyberspace Compound", 1280, 720, false);

	srand((unsigned int)time(NULL));

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);
	
	//Do a call to reset the timer before we begin our update loop.
	w.UpdateWindow();
	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	
	}
	return 0;
}