from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from slowapi import Limiter
from slowapi.util import get_remote_address
import asyncio, json, logging
from functools import lru_cache
import subprocess
from fastapi import Request
app = FastAPI()

# CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# Logging
logging.basicConfig(filename="api.log", level=logging.INFO)
logger = logging.getLogger(__name__)

# Rate limiting
limiter = Limiter(key_func=get_remote_address)

# Caching
@lru_cache(maxsize=500)
def run_engine(q):
    
    return subprocess.run(["./search_engine", q], capture_output=True, text=True).stdout


@app.get("/search")
@limiter.limit("30/minute")
def search(request: Request, q: str):

    logger.info(f"Search: {q}")

    if not q.strip():
        raise HTTPException(400, "Query empty")

    try:
        output = run_engine(q)
        return json.loads(output)

    except Exception as e:
        logger.error(str(e))
        raise HTTPException(500, "Search failed")