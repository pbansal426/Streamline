import React from "react";
import { Paper, Typography, Slider } from "@mui/material";

const Inspector = () => {
  return (
    <Paper elevation={3} style={{ height: "100%" }}>
      <Typography variant="h6">Inspector</Typography>
      <Typography variant="body1">Track Volume</Typography>
      <Slider defaultValue={50} />
    </Paper>
  );
};

export default Inspector;
