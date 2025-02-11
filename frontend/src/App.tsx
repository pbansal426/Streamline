import React from "react";
import { AppBar, Toolbar, Typography, Container, Grid, Button } from "@mui/material";
import { styled } from "@mui/system";

// Styled components using the `styled` API
const Root = styled("div")({
  display: "flex",
  flexDirection: "column",
  height: "100vh",
});

const Content = styled("div")({
  flex: 1,
  display: "flex",
  flexDirection: "row",
});

const TrackArea = styled("div")({
  flex: 3,
  padding: "16px",
  background: "#f5f5f5",
});

const Controls = styled("div")({
  flex: 1,
  padding: "16px",
  background: "#e0e0e0",
});

const Inspector = styled("div")({
  flex: 1,
  padding: "16px",
  background: "#d3d3d3",
});

const AppBarStyled = styled(AppBar)(({ theme }) => ({
  backgroundColor: theme.palette.primary.main,
}));

function App() {
  return (
    <Root>
      <AppBarStyled position="sticky">
        <Toolbar>
          <Typography variant="h6">Streamline DAW</Typography>
        </Toolbar>
      </AppBarStyled>

      <Content>
        <TrackArea>
          <Typography variant="h6">Track Area</Typography>
          {/* Track area content (audio tracks, mixer, etc.) */}
        </TrackArea>

        <Controls>
          <Typography variant="h6">Transport Controls</Typography>
          {/* Transport controls content (play, pause, stop buttons, timeline, etc.) */}
          <Button variant="contained" sx={{ margin: "8px 0" }}>Play</Button>
          <Button variant="contained" sx={{ margin: "8px 0" }}>Stop</Button>
        </Controls>

        <Inspector>
          <Typography variant="h6">Inspector</Typography>
          {/* Inspector content (selected track details, effects, etc.) */}
        </Inspector>
      </Content>
    </Root>
  );
}

export default App;
