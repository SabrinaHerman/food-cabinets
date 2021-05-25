from sys import argv
from AutoShareFacebook import AutoShare

script, post_url = argv
#can access variable post_url
caption_text = input("Type the caption here: ")

auto = AutoShare()

auto.auth(uname='caitlinstanton44@gmail.com', passd='Pap3r$tack$')
auto.process(post_url=post_url, caption_text=caption_text)
