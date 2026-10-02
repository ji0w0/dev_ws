import os
import time
import requests
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager

def download_google_images(keyword, max_count=100):
    save_folder = f"./{keyword}"
    if not os.path.exists(save_folder):
        os.makedirs(save_folder)

    options = webdriver.ChromeOptions()
    options.add_argument('--no-sandbox')
    options.add_argument('--disable-dev-shm-usage')
    options.add_argument("user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36")
    options.add_experimental_option("excludeSwitches", ["enable-automation"])
    options.add_experimental_option('useAutomationExtension', False)

    driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()), options=options)

    try:
        url = f"https://www.google.com/search?q={keyword}&tbm=isch"
        driver.get(url)
        
        print("\n" + "=" * 50)
        print("구글 이미지 페이지에 접속했습니다.")
        while True:
            user_input = input("크롤링을 시작하려면 's'를 입력하고 Enter를 누르세요: ").strip().lower()
            if user_input == 's':
                print("크롤링을 시작합니다!\n" + "=" * 50)
                break
            else:
                print("잘못 입력하셨습니다. 's'를 입력해 주세요.")

        # 1. 목표 개수를 채울 때까지 스크롤 및 '결과 더보기' 자동 클릭
        print(f"목표 수량({max_count}개) 확보를 위해 스크롤을 내리는 중입니다...")
        last_height = driver.execute_script("return document.body.scrollHeight")
        
        while True:
            # 페이지 끝까지 스크롤
            driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
            time.sleep(1.5)  # 이미지 로딩 대기

            # 현재 로드된 이미지 수 확인
            current_images = driver.find_elements(By.CSS_SELECTOR, "img")
            if len(current_images) >= max_count * 1.5:  # 필터링될 이미지 고려하여 1.5배 확보
                break

            new_height = driver.execute_script("return document.body.scrollHeight")
            
            # 더 이상 스크롤이 내려가지 않을 때 '결과 더보기' 버튼 찾기
            if new_height == last_height:
                try:
                    # 구글의 '결과 더보기' 버튼 클래스/태그 탐색
                    more_btn = driver.find_element(By.CSS_SELECTOR, ".mye4qd, input[type='button'], .LZ42")
                    if more_btn.is_displayed():
                        more_btn.click()
                        print("'결과 더보기' 버튼을 클릭했습니다.")
                        time.sleep(2)
                except Exception:
                    # 더 이상 로드할 이미지가 없으면 스크롤 중단
                    print("페이지의 끝에 도달했습니다.")
                    break
            last_height = new_height

        # 2. 이미지 URL 수집 및 다운로드
        images = driver.find_elements(By.CSS_SELECTOR, "img")
        print(f"\n총 {len(images)}개의 이미지 태그를 찾았습니다. 다운로드를 시작합니다...")

        downloaded = 0
        visited_urls = set()  # 중복 다운로드 방지

        for img in images:
            if downloaded >= max_count:
                break

            img_url = img.get_attribute('data-src') or img.get_attribute('src')

            if not img_url or not img_url.startswith('http'):
                continue

            if 'google' in img_url or 'gstatic.com/m/images' in img_url:
                continue

            if img_url in visited_urls:
                continue
            visited_urls.add(img_url)

            try:
                res = requests.get(img_url, headers={'User-Agent': 'Mozilla/5.0'}, timeout=5)
                if res.status_code == 200:
                    file_path = os.path.join(save_folder, f"{keyword}_{downloaded + 1}.jpg")
                    with open(file_path, 'wb') as f:
                        f.write(res.content)
                    downloaded += 1
                    print(f"[{downloaded}/{max_count}] 저장 완료: {file_path}")
            except Exception:
                continue

        print(f"\n최종 {downloaded}개의 이미지가 '{save_folder}' 폴더에 저장되었습니다.")

    finally:
        driver.quit()

if __name__ == "__main__":
    search_keyword = input("검색할 키워드를 입력하세요: ")
    count = int(input("다운로드할 이미지 개수를 입력하세요: "))
    
    download_google_images(keyword=search_keyword, max_count=count)