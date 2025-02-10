from fastapi import APIRouter
from pydantic import BaseModel
from langchain_ollama import OllamaLLM

# Define a Pydantic model for the input data
class PromptRequest(BaseModel):
    prompt: str

router = APIRouter()

# Initialize Ollama model
llm = OllamaLLM(model="deepseek-r1") 

@router.get("/")
def ai_home():
    return {"message": "AI Route is working!"}

@router.get("/test")
def ai_test():
    return {"message": "AI Test Endpoint"}

@router.post("/prompt")
async def generate_response(request: PromptRequest):
    try:
        prompt = request.prompt  # Get the prompt from the request body
        # Sending the prompt to Ollama and getting a response
        response = llm.invoke(prompt)
        
        return {"response": response}
    except Exception as e:
        return {"error": str(e)}
