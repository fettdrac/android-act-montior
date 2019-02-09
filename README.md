# android-act-montior

本人高中生（高二）一枚，我的渣英语写出来的渣文案，代码注释都是中文

我是个新手，小白级那种，各位看看我写的c++代码就知道了（C#和JAVA中毒已深，反射JAVA层方法超级多，虽然比较稳定的那种），而且我自己都看出了好几个栈溢出覆盖返回值，临界资源不加锁导致可以race的绕过漏洞（尤其是UserAuthReceiver.java里，那种文件交互）

总之一句话就是这玩意的lib通过libinject2注入zygote可以监控APP的行为，目前已经实现了拍照（camera1 camera2都可以拦截），定位拦截，并且发一个提示给用户让用户确认让不让app做操作的功能。

这里只上传关键代码，因为我电脑上的git不知道出了啥问题，一上传整个项目文件夹就崩（第一次在GITHUB上传代码）

再次感谢asLody,Rprop大佬提供的Hook Framework和代码中的思路指导

小白萌新，欢迎并希望dalao给出意见，协议就是用了里面我自己写的代码注明一下是我的就好了（应该也没人会用把）

This is a userApp activity(not a view) montior(a kind of supervision)
You can manually inject the lib(in SVision_Native.zip) to zygote to supervision any user layer app and custom their acticities in lib's code or create a auto tool to inject the lib automatically.

NOTICE:
I upload these file in *.zip because I cannot use Git to upload the whole project folders for some of the reason.
One of them is to prevent the really newers to copy my code totally and give up their own research and the other is that my computer's Git broke down 555~
I am a newer of Android deep layer world(ai...) with a poor programming level of C++ as well so some of the code may have some serious vuls(I have written some of them I have discovered within my ability (which is pretty poor as a matter of fact) in descriptions) that can bypass the UserAuth.

There are some pioneers which I am willing to mention and specially appericate.

asLody(too much, for instance Lenged)
RProp(his AndHook framework has given me a lot of help helping me save long time troubling on platform's command sets and HOOK)
LibInject2(You can found it in github too) to inject my lib to zygote or some other user layer app.

In a word, this is a combination of pioneers work and some of my own devlopment(very small~). It is my pleasure to receive pioneer's suggestions.


