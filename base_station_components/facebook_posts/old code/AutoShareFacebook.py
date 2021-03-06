'''
MIT License
Copyright (c) 2017 Sameer Kumar
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
'''

from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
import time


class AutoShare:
    def __init__(self):
        self.url = 'https://www.facebook.com'
        chrome_options = webdriver.ChromeOptions()
        prefs = {"profile.default_content_setting_values.notifications": 2}
        chrome_options.add_experimental_option("prefs", prefs)
        self.driver = webdriver.Chrome(chrome_options=chrome_options)
        self.driver.maximize_window()
        self.driver.get(self.url)
        self.fb_posting = [
            "/html/body/div[1]/div/div[1]/div/div[3]/div/div/div[1]/div[1]/div[4]/div/div/div/div/div[1]/div[1]/div/div/div/div[1]/div/div[1]",
            "/html/body/div[1]/div/div[1]/div/div[4]/div/div/div[1]/div/div[2]/div/div/div/div/div[1]/form/div/div[1]/div/div/div[1]/div[2]/div[1]/div[1]/div[1]/div/div/div/div/div[2]/div/div/div/div",
            "/html/body/div[1]/div/div[1]/div/div[4]/div/div/div[1]/div/div[2]/div/div/div/div/div[1]/form/div/div[1]/div/div/div[1]/div[3]/div[2]/div/div"
        ]

    def auth(self, uname, passd):
        time.sleep(2)
        username = self.driver.find_element_by_id('email')
        password = self.driver.find_element_by_id('pass')
        username.send_keys(uname)
        password.send_keys(passd)
        password.send_keys(Keys.ENTER)

    def process(self, post_url, caption_text):
        groups = []
        #groups = ['https://www.facebook.com/groups/bangalorestartupsnetwork/',
        #          'https://www.facebook.com/groups/delhistartupnetwork/',
        #          'https://www.facebook.com/groups/indianstartupnetwork']
        time.sleep(1)
        if not groups:
            try:
                number_of_groups = int(input("How many groups:"))
                for j in range(0, number_of_groups):
                    app = input("Enter group url(with https)" + str(j + 1) +
                                ":")
                    groups.append(app)
                pass
            except:
                print("Error")

        else:
            pass

        for i in groups:
            self.driver.get(i)
            time.sleep(5)
            self.driver.find_element_by_xpath(self.fb_posting[0]).click()
            time.sleep(2)

            self.driver.find_element_by_xpath(self.fb_posting[1]).send_keys(
                Keys.ENTER, caption_text)
            time.sleep(2)

            self.driver.find_element_by_xpath(self.fb_posting[2]).click()
            time.sleep(5)
            # post_box = self.driver.find_element_by_xpath(
            #     "//*[@name='xhpc_message_text']")
            # try:
            #     self.driver.find_element(
            #         By.XPATH, '//*[@label="Create a public post..."]').click()
            #     post_box = self.driver.find_element_by_css_selector(
            #         "[name='xhpc_message_text']")
            # except:
            #     post_box = self.driver.find_element_by_css_selector(
            #         "[name='xhpc_message_text']")

            # post_box.send_keys(caption_text)
            # time.sleep(5)

            # # Get the 'Post' button and click on it
            # Post_button = self.driver.find_element_by_xpath(
            #     "//*[@data-testid='react-composer-post-button']")
            # time.sleep(5)
            # Post_button.click()
            # time.sleep(5)
            # try:
            #     self.driver.get(str(i))
            #     actions = ActionChains(self.driver)
            #     time.sleep(2)
            #     actions.send_keys('p').perform()
            #     time.sleep(1)

            #     actions2 = ActionChains(self.driver)
            #     actions2.send_keys(str(post_url) + " ").perform()
            #     time.sleep(3)

            #     actions4 = ActionChains(self.driver)

            #     for j in range(0, 15):
            #         actions4.send_keys(Keys.BACK_SPACE).perform()

            #     caption = ActionChains(self.driver)
            #     caption.send_keys(caption_text).perform()

            #     click_post = self.driver.find_element_by_class_name("_332r")
            #     click_post.click()
            #     time.sleep(10)
            #     print("Successfully posted in " + str(
            #         i.split('https://www.facebook.com/groups/')[1].split('/')
            #         [0]))
            # except:
            #     print("Error posting in " + str(
            #         i.split('https://www.facebook.com/groups/')[1].split('/')
            #         [0]))
        self.driver.close()
