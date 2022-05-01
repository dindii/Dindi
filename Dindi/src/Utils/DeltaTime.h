#pragma once

namespace Dindi
{
	class DeltaTime
	{
	public:
		DeltaTime();
		void Tick();
		double getDeltaTime();

		operator float() { return (float)deltaTime; }
		operator double() { return deltaTime; }

	private:
		double currentTime;
		double previousTime;
		double deltaTime;
		double secondsPerCount;
	};
}