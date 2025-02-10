from fastapi import APIRouter

router = APIRouter()

@router.get("/")
def audio_home():
    return {"message": "Audio Route is working!"}
