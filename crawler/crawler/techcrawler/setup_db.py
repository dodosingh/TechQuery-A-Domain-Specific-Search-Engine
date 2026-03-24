import sqlite3
def create_database():
    conn=sqlite3.connect("crawler_data.db")
    cur=conn.cursor()

    cur.execute(
        """
    CREATE TABLE IF NOT EXISTS pages (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        url TEXT UNIQUE NOT NULL,
        domain TEXT,
        title TEXT,
        content TEXT,
        length INTEGER,
        category TEXT,
        crawled_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
    """

    )

    cur.execute("CREATE INDEX IF NOT EXISTS idx_url ON pages(url)")
    cur.execute("CREATE INDEX IF NOT EXISTS idx_category ON pages(category)")
    cur.execute("CREATE INDEX IF NOT EXISTS idx_domain ON pages(domain)")

    conn.commit()
    conn.close()
    print("database created successfully")

if __name__ == "__main__":
    create_database()