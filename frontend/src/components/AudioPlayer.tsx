import React, { useState } from "react";
import * as Tone from "tone";

const AudioPlayer = () => {
  const [audioFile, setAudioFile] = useState<File | null>(null);
  const [loading, setLoading] = useState(false);
  const [analysisResult, setAnalysisResult] = useState<any>(null);
  const [audioPlayer, setAudioPlayer] = useState<Tone.Player | null>(null);
  const [isPlaying, setIsPlaying] = useState(false);

  const handleAudioChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files) {
      setAudioFile(e.target.files[0]);
    }
  };

  const loadAudio = async () => {
    if (!audioFile) return;
    
    setLoading(true);
    try {
      const newPlayer = new Tone.Player();
      await newPlayer.load(URL.createObjectURL(audioFile));
      newPlayer.toDestination();
      setAudioPlayer(newPlayer);

      console.log("Audio Loaded");

      // Send file to backend for analysis
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
      setAnalysisResult(data);
    } catch (error) {
      console.error("Error loading audio:", error);
    } finally {
      setLoading(false);
    }
  };

  const handlePlay = () => {
    if (audioPlayer) {
      Tone.start();
      audioPlayer.start();
      setIsPlaying(true);
    }
  };

  const handleStop = () => {
    if (audioPlayer) {
      audioPlayer.stop();
      setIsPlaying(false);
    }
  };

  return (
    <div style={{ padding: "20px", fontFamily: "Arial, sans-serif", background: "#222", color: "#fff", borderRadius: "10px" }}>
      <h2>Audio Player & Analysis</h2>

      {/* Audio File Input */}
      <input type="file" onChange={handleAudioChange} disabled={loading} style={{ marginBottom: "10px" }} />

      {/* Load & Analyze Audio */}
      <button onClick={loadAudio} disabled={loading || !audioFile} style={{ marginRight: "10px" }}>
        {loading ? "Loading..." : "Load & Analyze Audio"}
      </button>

      {/* Play and Stop Buttons */}
      <button onClick={handlePlay} disabled={isPlaying || loading || !audioPlayer} style={{ marginRight: "5px" }}>
        ▶ Play
      </button>
      <button onClick={handleStop} disabled={!isPlaying || loading}>
        ⏹ Stop
      </button>

      {/* Display Analysis Results */}
      {analysisResult && (
        <div style={{ marginTop: "20px", padding: "10px", background: "#333", borderRadius: "8px" }}>
          <h3>Audio Analysis:</h3>
          <p><strong>Tempo:</strong> {analysisResult.bpm} BPM</p>
          <p><strong>Pitch:</strong> {analysisResult.pitch.toFixed(2)} Hz</p>
          <p><strong>Loudness:</strong> {analysisResult.loudness.toFixed(2)} dB</p>
          <p><strong>Key:</strong> {analysisResult.key} (Strength: {analysisResult.key_strength.toFixed(2)})</p>
          <p><strong>Genre:</strong> {analysisResult.genre}</p>
          <p><strong>Spectral Centroid:</strong> {analysisResult.spectral_centroid.toFixed(2)} Hz</p>
          <p><strong>Spectral Flux:</strong> {analysisResult.spectral_flux.toFixed(2)}</p>
          <p><strong>Dynamic Complexity:</strong> {analysisResult.dynamic_complexity.toFixed(2)}</p>

          {/* Display MFCC Summary */}
          {analysisResult.mfcc_coeffs && (
            <div>
              <h4>MFCC (Timbre) Summary:</h4>
              <p>Mean MFCC Coefficients: {analysisResult.mfcc_coeffs.slice(0, 5).map((val: number) => val.toFixed(2)).join(", ")}...</p>
            </div>
          )}
        </div>
      )}
    </div>
  );
};

export default AudioPlayer;
