import React, { useState } from "react";
import TrackSequencer from "./TrackSequencer";

const MusicProduction: React.FC = () => {
  const [showSequencer, setShowSequencer] = useState(false);

  return (
    <div style={{ padding: "20px", background: "#222", color: "#fff" }}>
      <h2>Music Production</h2>

      <button onClick={() => setShowSequencer(!showSequencer)}>
        {showSequencer ? "Hide Sequencer" : "Show Sequencer"}
      </button>

      {showSequencer && <TrackSequencer />}
    </div>
  );
};

export default MusicProduction;
