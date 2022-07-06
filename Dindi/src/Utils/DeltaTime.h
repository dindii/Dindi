#pragma once

namespace Dindi
{
	class DeltaTime
	{
	public:
		DeltaTime();
		void Tick();

		operator float() { return deltaTime; }
		//operator double() { return deltaTime; }

	private:
		float currentTime;
		float previousTime;
		float deltaTime;
		float secondsPerCount;
	};
}