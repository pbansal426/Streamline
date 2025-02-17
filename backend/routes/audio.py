from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from typing import List

router = APIRouter()

# Temporary storage for tracks (later can use a database)
tracks = []


class Track(BaseModel):
    id: int
    name: str
    instrument: str  # e.g., "synth", "sampler"


@router.post("/add_track")
async def add_track(track: Track):
    tracks.append(track)
    return {"message": "Track added successfully", "track": track}


@router.get("/get_tracks")
async def get_tracks():
    return tracks
