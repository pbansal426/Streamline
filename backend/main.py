from fastapi import FastAPI
from routes import ai, audio, user
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

# CORS Configuration (Allow frontend to connect)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Change this to your frontend URL later for security
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Include all routers
app.include_router(ai.router, prefix="/ai")
app.include_router(audio.router, prefix="/audio")
app.include_router(user.router, prefix="/user")

@app.get("/")
def home():
    return {"message": "Backend is running!"}
