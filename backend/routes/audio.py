from fastapi import APIRouter, File, UploadFile
import essentia.standard as es
import numpy as np
import tempfile

router = APIRouter()

@router.post("/analyze")
async def analyze_audio(file: UploadFile = File(...)):
    # Save the uploaded file temporarily
    with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as temp_file:
        temp_file.write(await file.read())
        temp_filename = temp_file.name

    # Load the audio file
    audio = es.MonoLoader(filename=temp_filename)()

    # Extract features using Essentia
    bpm_extractor = es.RhythmExtractor2013()
    bpm, _, _, _, _ = bpm_extractor(audio)

    pitch_extractor = es.PitchYinFFT()
    pitch, _ = pitch_extractor(audio)

    loudness_extractor = es.Loudness()
    loudness = loudness_extractor(audio)

    return {
        "bpm": bpm,
        "pitch": pitch,
        "loudness": loudness
    }
