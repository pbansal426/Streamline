import React from "react";
import { Button, Grid, Typography } from "@mui/material";

const TransportControls = () => {
  return (
    <Grid container spacing={2} direction="column" alignItems="center">
      <Typography variant="h6">Transport Controls</Typography>
      <Button variant="contained" color="primary">
        Play
      </Button>
      <Button variant="contained" color="secondary">
        Stop
      </Button>
      <Button variant="contained" color="default">
        Record
      </Button>
    </Grid>
  );
};

export default TransportControls;
