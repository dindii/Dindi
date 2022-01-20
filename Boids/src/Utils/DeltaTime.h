#pragma once

namespace Boids
{
	class DeltaTime
	{
	public:
		DeltaTime();
		void Tick();
		double getDeltaTime();

		operator float() { return (float)deltaTime; }

	private:
		long long int currentTime;
		long long int previousTime;
		double deltaTime;
		double secondsPerCount;
	};
}