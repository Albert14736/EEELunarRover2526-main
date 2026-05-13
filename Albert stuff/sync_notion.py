import requests
import json
import os

# ==========================================
# 1. 配置区：请填入你刚刚申请到的信息
# ==========================================
NOTION_TOKEN = "这里替换成你的 secret_ 开头的 Token"
# 这是你之前在链接里提供的子页面 ID
PAGE_ID = "35fff437f92080e0bc16e0a048389e13" 

# 要保存的本地文件路径 (直接保存在当前目录下的 frontend.html)
OUTPUT_FILE = os.path.join(os.path.dirname(__blank__), "frontend.html").replace("__blank__", __file__)

# ==========================================
# 2. 获取 Notion 数据的核心逻辑
# ==========================================
headers = {
    "Authorization": f"Bearer {NOTION_TOKEN}",
    "Notion-Version": "2022-06-28"
}

url = f"https://api.notion.com/v1/blocks/{PAGE_ID}/children"
print("正在连接 Notion 云端获取最新代码...")

response = requests.get(url, headers=headers)
data = response.json()

code_content = ""
for block in data.get("results", []):
    # 遍历页面上的所有内容，只提取 "代码块" (Code Block)
    if block["type"] == "code":
        code_texts = block["code"]["rich_text"]
        for text in code_texts:
            code_content += text["plain_text"]
        code_content += "\n\n" # 如果有多个代码块，用空行隔开

if code_content:
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write(code_content)
    print(f"✅ 成功！已将 Notion 中的代码同步至本地: frontend.html")
else:
    print("❌ 提取失败：请检查页面 ID 是否正确，或者该页面中是否真的有 '代码块'。")