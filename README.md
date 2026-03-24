Tech Domain Search Engine

A full-stack, tech-focused search engine built from scratch featuring automated web crawling, a custom C++ indexing core, and a responsive Flutter frontend.



🚀 Project Overview

This project implements a scalable search engine system designed specifically for technology and gadget-related content. It includes an automated crawler, persistent data storage, a high-performance C++ search engine core using inverted indexing and TF-IDF ranking, and a modern Flutter-based UI for fast and intuitive search interactions.

🏗 System Architecture
Web → Scrapy Crawler → SQLite Database
                ↓
        C++ Core Engine (Index + Ranking)
                ↓
            Backend API
                ↓
         Flutter Frontend UI

🧠 Core Features
🔎 Search Engine Core (C++)

Inverted Index implementation

Term Frequency (TF) ranking

TF-IDF scoring algorithm

Title relevance boost

Category-based filtering

Optimized query execution

Index persistence support

🕷 Web Crawler (Scrapy)

Automated seed-based crawling

Link discovery and depth control

Junk URL filtering

Duplicate prevention (UNIQUE constraints)

Periodic crawl scheduling support

🗄 Data Storage (SQLite)

Serverless, self-contained database

Structured page storage

Indexed metadata (URL, category, domain)

Efficient data retrieval for indexing

📱 Frontend (Flutter)

Clean and responsive UI

Search bar with real-time results

Category filtering options

Backend API integration

⚙️ Tech Stack

Crawler: Python (Scrapy + Playwright)

Database: SQLite

Core Engine: C++

Backend API: FastAPI / Flask (planned/optional)

Frontend: Flutter

🧮 Ranking Algorithm

The search engine implements:

Inverted Index

Term Frequency (TF)

TF-IDF scoring

Title-based score boosting

Category-based result filtering

📦 How to Run
1️⃣ Run Crawler
scrapy crawl mycrawler

2️⃣ Build C++ Engine
gcc -c sqlite3.c
g++ search_engine.cpp sqlite3.o -O2 -o search_engine

3️⃣ Run Search Engine
.\search_engine.exe

📈 Future Improvements

Incremental indexing

Index compression

Phrase search support

Autocomplete

Distributed crawling

API deployment

Cloud hosting

🎯 Learning Outcomes

Search engine architecture design

Systems programming in C++

Database-driven indexing

Full-stack integration

Performance optimization

Crawl scheduling and automation
