#pragma once

#include "Utils.h"
#include "FastSin.h"

namespace CloudSeed
{
	class Particle
	{
	public:
		double ReadIndex;
		double Increment;
		
		double LengthSamples;
		double IncrementedSamples;
	};

	class ParticleDelay
	{
	private:

		static constexpr int MaxParticles = 10;
		static constexpr int ModulationUpdateRate = 8;

		double* delayBuffer;
		double* output;
		double* particleOutput;
		double samplerate;
		Particle particles[MaxParticles];
		int bufferSize;
		int writeIndex;
		int readIndex;
		int delayBufferSizeSamples;
		int particleCount;

		double modPhase;
		int modulationUpdateCounter;
		int particleUpdateCounter;
		int nextParticleIndex;

	public:
		int SampleDelay;
		double ModAmount;
		double ModRate;
		
		double ParticleLengthMillis;
		double PitchShift;

		ParticleDelay(int bufferSize, double samplerate, int delayBufferSizeSamples, int sampleDelay)
		{
			modulationUpdateCounter = 0;
			particleUpdateCounter = 0;
			nextParticleIndex = 0;

			this->samplerate = samplerate;
			this->delayBufferSizeSamples = delayBufferSizeSamples;
			this->bufferSize = bufferSize;
			this->delayBuffer = new double[delayBufferSizeSamples];
			this->output = new double[bufferSize];
			this->particleOutput = new double[bufferSize];
			this->SampleDelay = sampleDelay;
			writeIndex = 0;
			modPhase = 0.01 + 0.98 * (std::rand() / (double)RAND_MAX);
			ModRate = 0.0;
			ModAmount = 0.0;
			particleCount = 2;
			ParticleLengthMillis = 100;
			PitchShift = 0;
			UpdateModulation();

			ResetParticles();
		}

		~ParticleDelay()
		{
			delete delayBuffer;
			delete output;
			delete particleOutput;
		}

		double* GetOutput()
		{
			return output;
		}

		double* GetParticleOutput()
		{
			return particleOutput;
		}

		void SetParticleCount(int count)
		{
			particleCount = count;
			ResetParticles();
		}

		void Process(double* input, int sampleCount)
		{
			auto particleGain = 1 / std::sqrt(particleCount);

			// this computes the frequency at which we need to trigger new particles, ensure we always have
			// a certain number of particles playing simultaneously
			auto particleUpdateTarget = ParticleLengthMillis / 1000.0 / particleCount * samplerate;

			for (int i = 0; i < sampleCount; i++)
			{
				if (modulationUpdateCounter >= ModulationUpdateRate)
					UpdateModulation();

				if (particleUpdateCounter >= particleUpdateTarget)
					TriggerParticle();

				delayBuffer[writeIndex] = input[i];

				double sum = 0.0;
				for (size_t i = 0; i < particleCount; i++)
				{
					sum += ProcessParticle(i);
				}

				output[i] = delayBuffer[readIndex];
				particleOutput[i] = sum * particleGain;
				
				writeIndex++;
				readIndex++;
				if (writeIndex >= delayBufferSizeSamples) writeIndex -= delayBufferSizeSamples;
				if (readIndex >= delayBufferSizeSamples) readIndex -= delayBufferSizeSamples;
				modulationUpdateCounter++;
				particleUpdateCounter++;
			}
		}

		void ClearBuffers()
		{
			Utils::ZeroBuffer(delayBuffer, delayBufferSizeSamples);
			Utils::ZeroBuffer(output, bufferSize);
			Utils::ZeroBuffer(particleOutput, bufferSize);
		}


	private:

		void ResetParticles()
		{
			auto particleLengthSamples = ParticleLengthMillis / 1000.0 * samplerate;

			for (size_t i = 0; i < MaxParticles; i++)
			{
				particles[i].ReadIndex = readIndex;
				particles[i].Increment = 1.0;
				particles[i].LengthSamples = particleLengthSamples;
				particles[i].IncrementedSamples = particleLengthSamples; // initialize particle as if it's already completed its iteration
			}
		}

		double ProcessParticle(int particleNum)
		{
			Particle* p = &particles[particleNum];
			if (p->IncrementedSamples >= p->LengthSamples)
				return 0.0;

			auto output = delayBuffer[(int)p->ReadIndex];
			output *= ParticleWindow(p->IncrementedSamples, p->LengthSamples);
			p->IncrementedSamples++;
			p->ReadIndex += p->Increment;
			if (p->ReadIndex >= delayBufferSizeSamples) 
				p->ReadIndex -= delayBufferSizeSamples;

			return output;
		}

		void TriggerParticle()
		{
			auto particleLengthSamples = ParticleLengthMillis / 1000.0 * samplerate;

			Particle* p = &particles[nextParticleIndex];
			p->Increment = 2;
			p->IncrementedSamples = 0;
			p->LengthSamples = particleLengthSamples * (0.9 + 0.1 * (std::rand() / (double)RAND_MAX));
			p->ReadIndex = readIndex;

			if (p->ReadIndex < 0)
				p->ReadIndex += delayBufferSizeSamples;
			else if (p-> ReadIndex >= delayBufferSizeSamples)
				p->ReadIndex -= delayBufferSizeSamples;

			nextParticleIndex++;
			if (nextParticleIndex >= particleCount)
				nextParticleIndex -= particleCount;
			particleUpdateCounter = 0;
		}

		void UpdateModulation()
		{
			/*modPhase += ModRate * ModulationUpdateRate;
			if (modPhase > 1)
				modPhase = std::fmod(modPhase, 1.0);

			auto mod = FastSin::Get(modPhase);
			auto totalDelay = SampleDelay + ModAmount * mod;*/

			auto totalDelay = SampleDelay;

			auto delayA = (int)totalDelay;

			readIndex = writeIndex - delayA;
			if (readIndex < 0) readIndex += delayBufferSizeSamples;

			modulationUpdateCounter = 0;
		}

		double ParticleWindow(int incSample, int len)
		{
			return 1.0;
			/*double ratio = incSample / (double)len;

			if (ratio < 0.5)
				return ratio * 2;
			else
				return 1 - (ratio - 0.5) * 2;*/
		}
	};
}
