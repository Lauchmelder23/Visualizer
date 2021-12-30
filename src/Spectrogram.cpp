#include "Spectrogram.hpp"

#include <complex>

#define POW_OF_TWO(x) ((x) && !((x) & ((x) - 1)))

std::vector<std::complex<float>> radix2dit(
    const std::vector<float> samples, 
    size_t offset,
    size_t N, 
    size_t s
);

Spectrogram::Spectrogram(
    lol::ObjectManager& manager, 
    const glm::vec2& size, 
    const glm::uvec2& subdivision,
    const AudioFile& audio
) :
    Topology(manager, size, subdivision), audio(audio)
{
    this->audio.Normalize();

    range = glm::vec2(0.0f, 0.0005f);
    MakeTexture();
} 

void Spectrogram::Update()
{
    // Load samples
    unsigned int sampleNumber = audio.GetAudioSpec().freq / 60;
    std::vector<float> samples(audio.begin() + currentStrip * sampleNumber, audio.begin() + (currentStrip + 1) * sampleNumber);
    size_t N = samples.size();

    // Zeropad the signal
    while(!POW_OF_TWO(N))
    {
        samples.push_back(0.0f);
        N++;
    }

    N = samples.size() << 3;
    samples.insert(samples.end(), N - samples.size(), 0.0f);

    // Perform Fourier transformation on the next samples
    std::vector<std::complex<float>> spectrum = radix2dit(samples, 0, N, 1);
    float freqRes = (float)audio.GetAudioSpec().freq / (float)N;
	float nyquistLimit = (float)audio.GetAudioSpec().freq / 2.0f;

	std::vector<std::pair<float, float>> output;
	float freq = 50.0f;
	float maxFreq = nyquistLimit;

	for (int k = freq / freqRes; freq < nyquistLimit && freq < maxFreq; k++)    // ??? wtf is going on here?
	{
		output.push_back(std::make_pair(freq, 2.0f * std::abs(spectrum[k]) / (float)N));

		freq += freqRes;
	}

    float* pixels = GetTopology();
    glm::uvec2 dims = image.GetDimensions();

    glm::vec2 arrayDomain(0.0f, N);
    glm::vec2 imageDomain(0.0f, dims.y);

    unsigned int imageStrip = currentStrip % dims.x;
    for(unsigned int y = 0; y < dims.y; y++)
    {
        std::complex<float> sample = output[Map(imageDomain, arrayDomain, y)].second;
        float magnitude = std::abs(sample);

        pixels[y * dims.x + imageStrip] = magnitude;
    }

    MakeTexture();

    currentStrip++;
    offset += 1.0f / (float)dims.x;
}

std::vector<std::complex<float>> radix2dit(
    const std::vector<float> samples, 
    size_t offset,
    size_t N, 
    size_t s
)
{
    std::vector<std::complex<float>> output(N);

    if(N == 1)
    {
        output[0] = samples[offset];
    }
    else
    {
        size_t halfN = N >> 1;
        std::vector<std::complex<float>> first = radix2dit(samples, offset, halfN, 2 * s);
        std::vector<std::complex<float>> second = radix2dit(samples, offset + s, halfN, 2 * s);

        float coeff = -M_PI / (float)halfN;

		for (int k = 0; k < halfN; k++)
		{
			std::complex<float> p = first[k];
			std::complex<float> q = std::exp(coeff * (float)k) * second[k];

			output[k] = p + q;
			output[halfN + k] = p - q;
		}
    }

    return output;
}