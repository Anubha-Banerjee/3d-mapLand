#include <stdio.h>
#include <irrKlang.h>
#include <conio.h>


using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class sounds
{
 

  char file[100];

  public:
	ISoundEngine* engine;
	sounds(char filename[100])
	{
		strcpy(file, filename);
		engine = createIrrKlangDevice();
		if (!engine)
		{
			printf("Could not startup engine\n");
			return;
		}
	}
	void stop()
	{
		engine->stopAllSounds();
	}
	void pause_play()
	{
		engine->setAllSoundsPaused();
	}
	
	void play(bool loop)
	{
		engine->play2D(file, loop);
	}
	
	~sounds()
	{
		engine->drop();
	}
};