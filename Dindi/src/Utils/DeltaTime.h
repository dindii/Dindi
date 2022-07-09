#pragma once

namespace Dindi
{
	class DeltaTime
	{
	public:
		DeltaTime();
		void Tick();

		operator float() { return deltaTime; }
		float Milliseconds() const { return deltaTime * 1000; }
	
	private:
		float currentTime;
		float previousTime;
		float deltaTime;
		float secondsPerCount;
	};
}