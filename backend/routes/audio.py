from fastapi import APIRouter, File, UploadFile
import essentia.standard as es
import numpy as np
import tempfile

router = APIRouter()

@router.post("/analyze")
async def analyze_audio(file: UploadFile = File(...)):
    with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as temp_file:
        temp_file.write(await file.read())
        temp_filename = temp_file.name

    # Load the audio
    audio = es.MonoLoader(filename=temp_filename)()

    # BPM (Tempo Detection)
    bpm_extractor = es.RhythmExtractor2013()
    bpm, _, _, _, _ = bpm_extractor(audio)

    # Pitch Detection
    pitch_extractor = es.PitchYinFFT()
    pitch, _ = pitch_extractor(audio)

    # Loudness
    loudness_extractor = es.Loudness()
    loudness = loudness_extractor(audio)

    # Key Detection
    key_extractor = es.KeyExtractor()
    key, scale, key_strength = key_extractor(audio)

    # Timbre Analysis (MFCCs & Spectral Features)
    mfcc_extractor = es.MFCC()
    mfcc_coeffs, _ = mfcc_extractor(audio)

    spectral_centroid_extractor = es.SpectralCentroidTime()
    spectral_centroid = spectral_centroid_extractor(audio)

    spectral_flux_extractor = es.Flux()
    spectral_flux = spectral_flux_extractor(audio)

    # Dynamic Complexity
    dynamic_complexity_extractor = es.DynamicComplexity()
    dynamic_complexity, _ = dynamic_complexity_extractor(audio)

    # Genre Classification
    music_extractor = es.MusicExtractor(lowlevelStats=["mean", "stdev"],
                                        rhythmStats=["mean", "stdev"],
                                        tonalStats=["mean", "stdev"])
    features = music_extractor(temp_filename)

    genre = features["metadata.tags.genre"][0] if "metadata.tags.genre" in features else "Unknown"

    return {
        "bpm": bpm,
        "pitch": pitch,
        "loudness": loudness,
        "key": f"{key} {scale}",
        "key_strength": key_strength,
        "spectral_centroid": spectral_centroid,
        "spectral_flux": spectral_flux,
        "dynamic_complexity": dynamic_complexity,
        "mfcc_coeffs": mfcc_coeffs.tolist(),
        "genre": genre
    }
