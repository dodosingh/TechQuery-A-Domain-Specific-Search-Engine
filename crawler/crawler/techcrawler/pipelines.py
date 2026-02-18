# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html

import sqlite3
from urllib.parse import urlparse

from itemadapter import ItemAdapter


class TechcrawlerPipeline:
    def open_spider(self, spider):
        db_path = spider.settings.get("SQLITE_DB_PATH", "crawler_data.db")
        self.conn = sqlite3.connect(db_path)
        self.cur = self.conn.cursor()

        self.cur.execute(
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
        self.cur.execute("CREATE INDEX IF NOT EXISTS idx_url ON pages(url)")
        self.cur.execute("CREATE INDEX IF NOT EXISTS idx_category ON pages(category)")
        self.cur.execute("CREATE INDEX IF NOT EXISTS idx_domain ON pages(domain)")
        self.conn.commit()

    def process_item(self, item, spider):
        adapter = ItemAdapter(item)

        url = adapter.get("url")
        if not url:
            return item

        domain = adapter.get("domain") or urlparse(url).netloc
        title = adapter.get("title")
        content = adapter.get("content")
        length = adapter.get("length")
        category = adapter.get("category") or "tech"

        self.cur.execute(
            """
            INSERT INTO pages (url, domain, title, content, length, category)
            VALUES (?, ?, ?, ?, ?, ?)
            ON CONFLICT(url) DO UPDATE SET
                domain=excluded.domain,
                title=excluded.title,
                content=excluded.content,
                length=excluded.length,
                category=excluded.category,
                crawled_at=CURRENT_TIMESTAMP
            """,
            (url, domain, title, content, length, category),
        )
        self.conn.commit()

        return item

    def close_spider(self, spider):
        if hasattr(self, "conn"):
            self.conn.close()
