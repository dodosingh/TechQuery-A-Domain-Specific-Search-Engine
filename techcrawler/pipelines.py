import sqlite3
from urllib.parse import urlparse
from itemadapter import ItemAdapter

PHONE_WORDS = {
    "phone","smartphone","iphone","android","mobile","handset",
    "samsung","oneplus","xiaomi","pixel","realme","oppo"
}

LAPTOP_WORDS = {
    "laptop","notebook","macbook","ultrabook",
    "dell","hp","lenovo","asus"
}

AUDIO_WORDS = {
    "headphone","earbuds","earphone","speaker",
    "audio","sound","bluetooth","mic"
}

class TechcrawlerPipeline:

    def open_spider(self, spider):
        db_path = spider.settings.get("SQLITE_DB_PATH", "crawler_data.db")
        self.conn = sqlite3.connect(db_path)
        self.cur = self.conn.cursor()
        self.count = 0

        self.cur.execute("""
            CREATE TABLE IF NOT EXISTS pages (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                url TEXT UNIQUE,
                domain TEXT,
                title TEXT,
                content TEXT,
                length INTEGER,
                category TEXT,
                crawled_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        """)

        self.cur.execute("CREATE INDEX IF NOT EXISTS idx_url ON pages(url)")
        self.cur.execute("CREATE INDEX IF NOT EXISTS idx_category ON pages(category)")
        self.cur.execute("CREATE INDEX IF NOT EXISTS idx_domain ON pages(domain)")
        self.conn.commit()

    def process_item(self, item, spider):

        adapter = ItemAdapter(item)

        url = adapter.get("url")
        if not url:
            return item

        domain = urlparse(url).netloc
        title = (adapter.get("title") or "").strip()
        content = (adapter.get("content") or "").strip()

        if len(content) < 300:
            return item

        text = (title + " " + content).lower()

        if any(w in text for w in PHONE_WORDS):
            category = "phone"
        elif any(w in text for w in LAPTOP_WORDS):
            category = "laptop"
        elif any(w in text for w in AUDIO_WORDS):
            category = "audio"
        else:
            category = "general"

        length = len(content)

        self.cur.execute("""
            INSERT INTO pages (url, domain, title, content, length, category)
            VALUES (?, ?, ?, ?, ?, ?)
            ON CONFLICT(url) DO UPDATE SET
                domain=excluded.domain,
                title=excluded.title,
                content=excluded.content,
                length=excluded.length,
                category=excluded.category,
                crawled_at=CURRENT_TIMESTAMP
        """, (url, domain, title, content, length, category))

        self.count += 1
        if self.count % 50 == 0:
            self.conn.commit()

        return item

    def close_spider(self, spider):
        self.conn.commit()
        self.conn.close()