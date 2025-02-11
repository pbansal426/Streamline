import React from "react";
import { List, ListItem, ListItemText, Divider } from "@mui/material";

const TrackList = () => {
  return (
    <List sx={{ width: "100%", maxWidth: 240 }}>
      <ListItem button>
        <ListItemText primary="Track 1" />
      </ListItem>
      <Divider />
      <ListItem button>
        <ListItemText primary="Track 2" />
      </ListItem>
      <Divider />
      <ListItem button>
        <ListItemText primary="Track 3" />
      </ListItem>
      <Divider />
      {/* Add more tracks dynamically here */}
    </List>
  );
};

export default TrackList;
