# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class TechcrawlerItem(scrapy.Item):
    product_name = scrapy.Field()
    brand = scrapy.Field()
    specifications=scrapy.Field()
    reviews=scrapy.Field()
    price = scrapy.Field()
    specifications = scrapy.Field()
    url = scrapy.Field()
    source = scrapy.Field()