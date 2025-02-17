import React, { useState, useEffect } from "react";
import * as Tone from "tone";

const STEPS = 16; // 16-step sequencer
const NOTES = ["C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"];

interface SequencerNote {
  step: number;
  note: string;
  active: boolean;
}

const TrackSequencer: React.FC = () => {
  const [sequence, setSequence] = useState<SequencerNote[][]>(() =>
    NOTES.map((note) =>
      Array(STEPS)
        .fill(null)
        .map((_, step) => ({ step, note, active: false }))
    )
  );

  const synth = new Tone.Synth().toDestination();
  const [isPlaying, setIsPlaying] = useState(false);

  useEffect(() => {
    const seq = new Tone.Sequence(
      (time, step) => {
        sequence.forEach((row) => {
          if (row[step].active) {
            synth.triggerAttackRelease(row[step].note, "8n", time);
          }
        });
      },
      Array.from({ length: STEPS }, (_, i) => i),
      "16n"
    );

    Tone.Transport.bpm.value = 120;
    if (isPlaying) {
      seq.start();
      Tone.Transport.start();
    } else {
      seq.stop();
      Tone.Transport.stop();
    }

    return () => seq.dispose();
  }, [isPlaying, sequence]);

  const toggleNote = (rowIdx: number, stepIdx: number) => {
    const newSequence = [...sequence];
    newSequence[rowIdx][stepIdx].active = !newSequence[rowIdx][stepIdx].active;
    setSequence(newSequence);
  };

  return (
    <div style={{ padding: "20px", background: "#222", color: "#fff", borderRadius: "10px" }}>
      <h2>Track Sequencer</h2>

      <div style={{ display: "grid", gridTemplateColumns: `repeat(${STEPS}, 30px)`, gap: "4px", marginBottom: "10px" }}>
        {sequence.map((row, rowIdx) =>
          row.map((cell, stepIdx) => (
            <div
              key={`${rowIdx}-${stepIdx}`}
              onClick={() => toggleNote(rowIdx, stepIdx)}
              style={{
                width: "30px",
                height: "30px",
                background: cell.active ? "#ff0" : "#444",
                borderRadius: "5px",
                cursor: "pointer",
              }}
            />
          ))
        )}
      </div>

      <button onClick={() => setIsPlaying(!isPlaying)}>
        {isPlaying ? "Stop" : "Play"}
      </button>
    </div>
  );
};

export default TrackSequencer;
