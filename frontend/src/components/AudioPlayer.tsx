import React, { useState, useEffect } from "react";
import * as Tone from "tone";

const AudioPlayer = () => {
  const [audioFile, setAudioFile] = useState<File | null>(null); // Audio file state
  const [loading, setLoading] = useState(false); // Loading state
  const [analysisResult, setAnalysisResult] = useState<any>(null); // Analysis result (tempo, etc.)
  const [audioPlayer, setAudioPlayer] = useState<Tone.Player | null>(null); // Tone.js player
  const [isPlaying, setIsPlaying] = useState(false); // Track if the audio is playing

  // Handle audio file change
  const handleAudioChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files) {
      setAudioFile(e.target.files[0]); // Save selected audio file
    }
  };

  // Function to load and play audio using Tone.js
  const loadAudio = async () => {
    setLoading(true);
    try {
      if (audioFile) {
        const newPlayer = new Tone.Player(); // Create new Tone.js player instance
        await newPlayer.load(URL.createObjectURL(audioFile)); // Load audio from file input
        newPlayer.toDestination(); // Connect to output (speakers)
        setAudioPlayer(newPlayer); // Save the player instance
        console.log("Audio Loaded");

        // Send audio file to backend for analysis
        const formData = new FormData();
        formData.append("file", audioFile);

        const res = await fetch("http://127.0.0.1:8000/audio/analyze/", {
          method: "POST",
          body: formData,
        });

        if (!res.ok) {
          throw new Error("Failed to analyze audio");
        }

        const data = await res.json();
        setAnalysisResult(data); // Set analysis result (e.g., tempo)
      }
    } catch (error) {
      console.error("Error loading audio:", error);
    } finally {
      setLoading(false);
    }
  };

  const handlePlay = () => {
    if (audioPlayer) {
      Tone.start(); // Ensure audio context is started (especially for mobile browsers)
      audioPlayer.start(); // Start playing the audio
      setIsPlaying(true); // Set playing state
    }
  };

  const handleStop = () => {
    if (audioPlayer) {
      audioPlayer.stop(); // Stop playing the audio
      setIsPlaying(false); // Set stop state
    }
  };

  return (
    <div>
      <h2>Audio Player & Analysis</h2>

      {/* Audio File Input */}
      <input type="file" onChange={handleAudioChange} disabled={loading} />

      {/* Load & Analyze Audio */}
      <button onClick={loadAudio} disabled={loading || !audioFile}>
        {loading ? "Loading Audio..." : "Load & Analyze Audio"}
      </button>

      {/* Play and Stop Buttons */}
      <div>
        <button onClick={handlePlay} disabled={isPlaying || loading || !audioPlayer}>
          Play Audio
        </button>
        <button onClick={handleStop} disabled={!isPlaying || loading}>
          Stop Audio
        </button>
      </div>

      {/* Display Analysis Result (Tempo, etc.) */}
      {analysisResult && (
        <div>
          <h3>Audio Analysis:</h3>
          <p>Tempo: {analysisResult.bpm} BPM</p>
          <p>Pitch: {analysisResult.pitch}</p>
          <p>Loudness: {analysisResult.loudness}</p>
        </div>
      )}
    </div>
  );
};

export default AudioPlayer;
