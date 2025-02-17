import React from "react";
import { AppBar, Toolbar, Typography, Button, Box } from "@mui/material";
import { styled, ThemeProvider, createTheme } from "@mui/material/styles";
import TrackList from "./components/TrackList";
import TrackArea from "./components/TrackArea";
import TransportControls from "./components/TransportControls";
import Inspector from "./components/Inspector";
import MusicProduction from "./components/MusicProduction";

// Define a theme
const theme = createTheme({
  palette: {
    primary: { main: "#111" },
    secondary: { main: "#555" },
    background: { default: "#1e1e1e", paper: "#2a2a2a" },
    text: { primary: "#fff", secondary: "#aaa" },
  },
});

// Styled components
const Root = styled("div")({
  display: "flex",
  flexDirection: "column",
  height: "100vh",
  backgroundColor: "#1e1e1e",
  fontFamily: "'Inter', sans-serif",
});

const AppBarStyled = styled(AppBar)({
  backgroundColor: "#111",
  height: "50px",
  boxShadow: "0px 2px 10px rgba(0, 0, 0, 0.3)",
});

const MainContent = styled("div")({
  display: "flex",
  flex: 1,
  flexDirection: "row",
  overflow: "hidden",
});

const TrackHeader = styled("div")({
  width: "260px",
  background: "#252525",
  color: "#fff",
  display: "flex",
  flexDirection: "column",
  padding: "12px",
  borderRight: "1px solid #444",
  borderRadius: "10px 0 0 10px",
});

const Timeline = styled("div")({
  flex: 1,
  background: "linear-gradient(180deg, #1a1a1a, #222)",
  position: "relative",
  overflow: "auto",
  borderRadius: "0 10px 10px 0",
});

const BottomPanel = styled("div")({
  height: "140px",
  background: "#222",
  display: "flex",
  flexDirection: "row",
  boxShadow: "0px -2px 10px rgba(0, 0, 0, 0.3)",
});

const LeftPanel = styled("div")({
  width: "320px",
  background: "#2d2d2d",
  padding: "10px",
  color: "#fff",
  borderRight: "1px solid #444",
  borderRadius: "10px 0 0 10px",
});

const RightPanel = styled("div")({
  width: "320px",
  background: "#2d2d2d",
  padding: "10px",
  color: "#fff",
  borderLeft: "1px solid #444",
  borderRadius: "0 10px 10px 0",
});

function App() {
  return (
    <ThemeProvider theme={theme}>
      <Root>
        {/* Top Bar */}
        <AppBarStyled position="sticky">
          <Toolbar>
            <Typography variant="h6" sx={{ fontWeight: "bold", letterSpacing: "1px" }}>
              Streamline DAW
            </Typography>
            <Box flexGrow={1} />
            <Button color="inherit" sx={{ mx: 1 }}>File</Button>
            <Button color="inherit" sx={{ mx: 1 }}>Edit</Button>
            <Button color="inherit" sx={{ mx: 1 }}>View</Button>
          </Toolbar>
        </AppBarStyled>

        {/* Main Content Area */}
        <MainContent>
          {/* Left Panel (Track Headers) */}
          <MusicProduction />
          <TrackHeader>
            <Typography variant="h6" sx={{ fontWeight: "bold", mb: 1 }}>
              Tracks
            </Typography>
            <TrackList />
          </TrackHeader>

          {/* Timeline (Track Arrangement) */}
          <Timeline>
            <TrackArea />
          </Timeline>

          {/* Right Panel (Inspector) */}
          <RightPanel>
            <Inspector />
          </RightPanel>
        </MainContent>

        {/* Bottom Transport & Mixer */}
        <BottomPanel>
          <LeftPanel>
            <Typography variant="h6" sx={{ fontWeight: "bold", mb: 1 }}>
              Library
            </Typography>
          </LeftPanel>

          <Box flex={1} display="flex" justifyContent="center" alignItems="center">
            <TransportControls />
          </Box>

          <RightPanel>
            <Typography variant="h6" sx={{ fontWeight: "bold", mb: 1 }}>
              Mixer
            </Typography>
          </RightPanel>
        </BottomPanel>
      </Root>
    </ThemeProvider>
  );
}

export default App;
