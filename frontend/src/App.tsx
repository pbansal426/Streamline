import React from "react";
import { AppBar, Toolbar, Typography, Button, Box } from "@mui/material";
import { styled, ThemeProvider, createTheme } from "@mui/material/styles";
import TrackList from "./components/TrackList";
import TrackArea from "./components/TrackArea";
import TransportControls from "./components/TransportControls";
import Inspector from "./components/Inspector";
import AudioPlayer from "./components/AudioPlayer";
import AIComponent from "./components/AIComponent";

// Define a theme
const theme = createTheme({
  palette: {
    primary: { main: "#333" },
    secondary: { main: "#555" },
    background: { default: "#222", paper: "#333" },
    text: { primary: "#fff", secondary: "#aaa" },
  },
});

// Styled components
const Root = styled("div")({
  display: "flex",
  flexDirection: "column",
  height: "100vh",
  backgroundColor: "#1e1e1e",
});

const AppBarStyled = styled(AppBar)({
  backgroundColor: "#111",
  height: "50px",
});

const MainContent = styled("div")({
  display: "flex",
  flex: 1,
  flexDirection: "row",
});

const TrackHeader = styled("div")({
  width: "240px",
  background: "#252525",
  color: "#fff",
  display: "flex",
  flexDirection: "column",
  padding: "8px",
});

const Timeline = styled("div")({
  flex: 1,
  background: "#1a1a1a",
  position: "relative",
  overflow: "hidden",
});

const BottomPanel = styled("div")({
  height: "120px",
  background: "#222",
  display: "flex",
  flexDirection: "row",
});

const LeftPanel = styled("div")({
  width: "300px",
  background: "#2d2d2d",
  padding: "8px",
  color: "#fff",
  borderRight: "1px solid #444",
});

const RightPanel = styled("div")({
  width: "300px",
  background: "#2d2d2d",
  padding: "8px",
  color: "#fff",
  borderLeft: "1px solid #444",
});

function App() {
  return (
    <ThemeProvider theme={theme}>
      <Root>
        {/* Top Bar */}
        <AppBarStyled position="sticky">
          <Toolbar>
            <Typography variant="h6">Streamline DAW</Typography>
            <Box flexGrow={1} />
            <Button color="inherit">File</Button>
            <Button color="inherit">Edit</Button>
            <Button color="inherit">View</Button>
          </Toolbar>
        </AppBarStyled>

        {/* Main Content Area */}
        <MainContent>
          {/* Left Panel (Track Headers) */}
          <AIComponent />
          <AudioPlayer />
          <TrackHeader>
            <Typography variant="h6" gutterBottom>Tracks</Typography>
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
            <Typography variant="h6">Library</Typography>
          </LeftPanel>

          <Box flex={1} display="flex" justifyContent="center" alignItems="center">
            <TransportControls />
          </Box>

          <RightPanel>
            <Typography variant="h6">Mixer</Typography>
          </RightPanel>
        </BottomPanel>
      </Root>
    </ThemeProvider>
  );
}

export default App;
