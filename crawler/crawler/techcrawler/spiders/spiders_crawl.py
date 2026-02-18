import scrapy
from scrapy.spiders import CrawlSpider
from urllib.parse import urlparse



class GadgetSpider( CrawlSpider):
    KEYWORDS = [
    # General tech terms
    "tech", "technology", "gadget", "device", "electronics",
    
    # Categories
    "smartphone", "mobile", "laptop", "tablet", "wearable", "smartwatch",
    "headphones", "earbuds", "camera", "tv", "monitor", "router",
    
    # Brands
    "apple", "samsung", "oneplus", "xiaomi", "redmi", "realme",
    "vivo", "oppo", "motorola", "google", "asus", "lenovo", "hp", "dell",
    
    # Specs and features
    "processor", "chipset", "ram", "storage", "display", "battery", "charging",
    "camera", "megapixel", "specifications", "benchmark", "performance",
    
    # Related topics
    "review", "launch", "price", "release", "comparison", "leak"
]
    USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36"
    

    name ="mycrawler"
    

    start_urls  =[

  "https://www.reliancedigital.in/",
  "https://www.croma.com/",
  "https://www.vijaysales.com/",
 

   "https://www.techradar.com",

    "https://www.gadgets360.com",

    "https://www.techspot.com",

    "https://www.androidauthority.com",

    "https://www.anandtech.com",

    "https://www.xda-developers.com",

    ]
    visited = set()
    
    def start_requests(self):
            for url in self.start_urls:
                yield scrapy.Request(
                    url,
                    meta={
                        "playwright": True,
                        "playwright_page_methods": [
                            # Example: wait until network idle or page fully loaded
                            {"method": "wait_for_load_state", "args": ["networkidle"]},
                        ],
                    },
                    callback=self.parse,
                )
    

    def parse(self, response):
            # Extract all links on the page
            print("PARSE HIT:", response.url)

            
            links = response.css("a::attr(href)").getall()

            for link in links:
                full_url =response.urljoin(link)

                if full_url in self.visited:
                    continue
                self.visited.add(full_url)
                yield response.follow(full_url, callback=self.parse_page)

                
    def parse_page(self, response):
        print("crawler working yield okk")
        text = " ".join(response.css("p::text").getall()).lower()
        self.logger.info("crawler working yield okk ")
        self.logger.info(f"SAVED DEAR: {response.url}")
        yield {
            "url": response.url,
            "title": response.css("title::text").get(),
            "content": text,
            "length": len(text),
        }
        print("crawler working yield okk")
        print("SAVED:", response.url)


