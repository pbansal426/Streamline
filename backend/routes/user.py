from fastapi import APIRouter

router = APIRouter()

@router.get("/")
def user_home():
    return {"message": "User Route is working!"}
