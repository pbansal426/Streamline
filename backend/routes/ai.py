from fastapi import APIRouter
from langchain_ollama import OllamaLLM

router = APIRouter()

# Initialize Ollama model
llm = OllamaLLM(model="deepseek-r1:1.5b") 


@router.get("/")
def ai_home():
    return {"message": "AI Route is working!"}

@router.get("/test")
def ai_test():
    return {"message": "AI Test Endpoint"}

@router.get("/generate/{prompt}")
def generate_response(prompt: str):
    try:
        # Sending the prompt to Ollama and getting a response
        response = llm.invoke(prompt) 
        return {"response": response}
    except Exception as e:
        return {"error": str(e)}

