# shooting range
 졸업 작품 용으로 만들어졌으나, 그냥 버리기는 아까운 면이 있어서 상당수의 코드를 갈아엎었습니다.      
졸업 작품 용은 old branch에서 확인할 수 있습니다.      
현재 정상적으로 실행되는 코드는 main branch에서 확인할 수 있습니다.     
그 외, 간간히 작업한 코드는 dev branch에 올리고 있습니다.                  
         
 ### 설명     
 ---
 언리얼 엔진을 사용한 FPS 트레이닝 게임입니다.    
제작 목적은 "FPS 게임 입문자가 FPS 장르에 좀 더 쉽게 적응하기 위해서 기본적인 시스템을 연습할 수 있게 한다." 입니다.       
이런 트레이닝 게임 특성상 실제 게임에서의 괴리감을 줄이는 것이 중요하다고 생각하여      
3가지 게임이 사용하는 시스템을 최대한 따라 해봤습니다.        
대상이 된 게임은 아래와 같습니다.    
  1. 배틀필드 4     
  2. 레인보우 식스 시즈     
  3. 이스케이프 프롬 타르코프     
      
선정된 각 게임들은 투사체, 히트스캔, 총알 발사 위치, 랜덤스프레드 여부 등 서로 겹치지 않는 구현을 가지기 때문에 적절하다고 판단했습니다.      

애니메이션의 경우 직접 만들었기 때문에 어색합니다.     
  
![image](https://drive.google.com/uc?export=view&id=1nsRe2RYn21QhJRpyaOew34hlI6FnhERx)
### 영상
---    
#### 배틀필드    
<div style="display:flex"> <!--배틀필드 -->
    <!-- 원본 -->
    <div style="flex:1;padding-right:10px;">
      <img src="https://drive.google.com/uc?export=view&id=1nsRe2RYn21QhJRpyaOew34hlI6FnhERx" width="600"/>
    </div>
    <!-- 구현 -->
    <div style="flex:1;padding-left:10px;">
      <img src="https://lh3.googleusercontent.com/fife/AAbDypDbE9O7-l_i1UiUIhRb2DNXNGThV0wTFENf4f8QsMBhlPxsABXy-m0FEtQ20izoEFAtE3AemiKbPa90xCTMxhor71Tl_UqfSPyVEb0BqDZnI4irkjAlSaigt3Xpfs69BuU40sbudtI2Mh4Ve2EDJ_SKRYsGmghhPMidlyK-9ZwMQzdwGTo5g9-rB4LQThBW7Rw2Pu5xHq4CWAAZO7ld5n50gacKXe3iF-pDwUxMWRILUjvXXVIX12QqjTL_h36f6rmVaT0HnMUCr_aVUbbGRMM93uegNizQd9AHidvas1v6nIb5mgAc-2cJWpMoENNNeNwQEUfGZnfYswGISfo4T8p6Yot41qktHZDH9QAw2Zzv29F4D53KJjk8CoYh4C9Q2Aq197W_pe_0-WLTFwTX9dD7ErWNdVpB-7L_6LSB_ZkhePWuOr0PGvd66R9mzW7ZhrFrS2trDEUG3QDv9UQ2GNCaWvOhaw5thUwIaoE43gjV1ZXU7F9foZeiLyqXyCzxeOTxhyKXn4VKBWZAj5wl8Mk2ik7mboJnaDlczvHNfa0Y2BJ0QZLNnShRuFF9vl5WGz-wNi1t-r4Dbkj3UxEWspih2SaZaqe7V66EnOHBdBZIlCzRnhVuqMyxVID2CdWClyn7wh_5EOIAmXAh5T7ipd53b5uLlZ_sMrzbbIPdRXdL_sCKY0IqYTh9M-rHukyKXKDMBmyqRXOHjLtpMDKvouKXmtXUaGN6P8cpRZTtDN7y5JfeJddWH2ibNZBMBL0ZFlHK8GWpoRgGtclN516FzASBR3WoBOPb1sifZ1ik7YmZuREb2OnJ0GucQ4PFERydLE-rU0_DCZpORxd_CfxLhKZABXaxQvcW-e9z4kq3lgMM5FBRHhu6O7keBnDxLAnxxSTz8Ttq5uTg0TSl6GjdyqZq362XgrTcJZ4UX7zJf138_VV--E8up3APRlNUOO57hwk99BwtPji7tviE9iQTVH6CNVLwQP-ydcgP1DOYoAsE2zK0Zi_e4oBDL9udzKtwYoI8vHTf5UzGgmZ9p1-70xFYXFNERN-6qYqqhXm0kbaQiLCh4OV4oipXvHBM3i454UiAYw1uRwoypAKXsmFZ9FEPOedRExvD1y4X5dBEY0WMC_DGz7Wl-wNaKco0aBD27Z_Ec8zCAi9BSAEvsx61mezfm8C1c4zdfj43keT1f7wcaP1wkVpQXBuLXwQKUxGh6NrXEJ7s5fSrvPdoH1s4gsQfFp8UGehy8i0e67X24OeXEav8x1X4V5GdKLrssuPfK9cpEUSJmzAYKSk01f_2UxDgqKkGd5uP7gmYIrNiy7wNATa3OqoQwEA-ZOiOcOzi78hjQhK9FEyyLaVhIgZH4JNfZwAvlqnbrfQZYmBFxwATGuSQjLEdTpUiy8BEp_8sMP8cVKU4e52KXieUalrp8sGxR1HnvySQx4xY_SiJD1MfFXtVs5vAh_o7w-JdAHZJLHoegA2rIPqu5tl5mArRblDVvlgbF6HBWrEbwOk_BG0DqT34ZD8M1ojfDJE=w1255-h738" width="600"/>
    </div> 
</div>

#### 레인보우식스 시즈
<div style="display:flex"> <!--레인보우 식스 시즈 -->
    <!-- 원본 -->
    <div style="flex:1;padding-right:10px;">
        <img src="https://lh3.googleusercontent.com/fife/AAbDypCwXdJobyx4GGTpCwh9jGYdJa-9y0DDTmSaq2LzekvY2BA03djvgX0x7ryiRW6c94j_Vs7SHvpiPRrWDt6OSgE5LqgqqZVlzKnDwF3wVIQc0sClvYjfms5L9qfAvFo2O2lL3gasuhxWCpphrztLI5OwncHSSWL5FLEqOARQ8jMdnr7UU5fQt-3lIjRlNBrR8KpmeS_sQvgwLA4yvzqm-Qii7oFJeYh_boABVekZrd_obv3kF-cWQVHZflYGHNyza6kT9KD1WOB5vY1sKE8B2R3KG0s7ECN8ItP9RBRCIjxZrwpm4HU-RH_SiG4hA_-s7pzhlnggV324uhP8JAVzuUI3uH8s4Pa1rArgawz0j_pDjKAzFGnnSUDH7nS-gkI21YDWk_YpjvI9Okhbm9DueRy5AkgqaJ7cn5qsikICY-s2StQ97i7KvvavS0oZVeuCp7Txt9Fb3F4hfSHAm6wGqo4K6AzF943NuKG3D5Fvx10kuumCWXbzH0QG7jay8Yd5suvRoVfS4supivB451pZOiQsU0AQ1Q0T9a1jog5XK51Gl_tyQoZ2I4j0hbBBGBuDP9Yf1-hVFtuJ12J6ACuCylZ2jJ3du6eBmNJt1ylF28nnSBuja8yUYAXaBxGGCWP5gdBGWLc_lPvygeV9rKDYBs3HvP5fU6sHVonpnpge5jHpBXN6vzcR5hHg3cbpLapyEoSRn3-j6LdytA_fT_HyyBVv9ZU00jvj_XZpF0BgGIE2g2My-jY11XQs52HNV33vt0WOsQogvXGWzxIFb8EHPZ1rAbu2yHNC0tOhB-h_estfOO9ATLD-qykhBgEgTg5JNVKwB_nO4FGxLXU13bMekSdsVU2rOpC_4lanR9k42JQQ47LMeQqyFtgRrXkZM7wntUws2lv97qNHW5PjJuoL20AX7maF_iyym_6GbifoXbqHr-xwvs3fhzLXr-lwEeXb9MgjBGwrKyl7fIlYcv4T5uD6L-a55gSX7XeGsZGVw-9tzXXI9zT8cBXRUWnVUmEdFHUat3eQ7E0oZxcg366uAZx5_6ZCbBj-4Q5qBcjclkm-MtM3VPtdux_bBwWgqtQ7vYwdVuc6SiJOLkzg8lJ6bzcgPW_3YT-KpKrT2mfoIXKz78ZfSWSxwD7Rmx3WLe1TDICcJyyaFDddR0oBsgl478qQq_J3hf2JrthbId6giHERK0ThjktV6WVsE7ZC0RJ9P-LWfnec-W6yiTtJx3PKFSnynUwaS_Rhs1inHNJeieyMceUUqw_0qe7O6V_OS8oXYuCyT3qUlLuIKK19sYdyzL-uJWMwz1vMl7Te1xUZsrIKiQb_cNOlvebP-90Hm2519ubLWEALKt5UpSoZe4j2KBDDoZZmDq7xpwdsnt5TRjJKBe4gCGF9Of6STR5n52VduT-4FDiWlSv5RO7qGsGJtSsIQcPhaurd3FlhC3vRLwHGAnMbybP_HjIhhKwBXm_u51kcqnORBsao8iMd7eIVh71RyD_KAZu1EL_-DaXyrYGLR0SRNni6GhfHNZ4=w1255-h738" width="600"/>
    </div>
    <!-- 구현 -->
    <div style="flex:1;padding-left:10px;">
        <img src="https://lh3.googleusercontent.com/fife/AAbDypBnT-F_5jS-t3b3USfj5gIEX_fVs6i7T1eV-wHrjLqKQjtD-NuEQbjHG-iEr3hKDFgZdbXsEAos0RY--JhYdl5fsVLs_gh2qPhgVquFzUF85KRYySMS6AKLKtMZz-hn5z2jypN7ACphey0S2yqrQP_5sqjd-waTGzxR7XNzWN5XQXrsivEoMVDe7L0CB-sd-HEK0hYdFUlm8zFjMC3wjpqXqVAqagyA53m41iwYH9hfh4RjKls5jYNMmnMOCJdgc1lFwRhpDSwnTiMXhItXKCLar3uR3gK3jG8C30bpU69GMomdFFtgv9w4rev8xy4h5nmy-8pK89Svj0HxZB5xK54qtxSQB8bRmwN8K1OMFB8QpWK1tS3gitdWPh_p8xTF7UI8w0_XxPn5CHcn8BR-dCf7q9FSgl8pA3iIyv8wW8j9XW-Mjc1lIvdtbRwFYO_TmQDgOWvf24sDXN4e6TjJ1wfM2wVG3UURHp1jyNxj6D-KexdqdwhQGANUmVobFZiC2HaSb4Y-i1ZbL6y4IOFW3r9fia_WrJr2i0KTAOvTaluJqkm2NMcyXB21bexqxcG6amJTEgjlikChsaDYRtNGOUC-RfgaAe2_bHJCyk6lYzdpXtB0gyEx8mo3ybdW1bJWjkvAR7ANt2EJ9X8iwRcUJ8mehJnWgyIBMZwnUDl_0IBne32lIXyxO-yaT_ytCJDJgJ30m3-UYr8xJxt6ld1IjkMpeqQZbaaTdgv7o2mCVqeaNgmt9OvzhK4PniW2P1Po0Mj5rKTF8ApUhWIDHJhwfh7tS6JTUyqXy7oUQ7z0lbytPqXXxgsmWwpYQu-_CNHvEPBDUQ39Miwr8bkYnhyp5LnwLdQiYcnqfho9AbsyU8KslfNvTAvUBeJv2tsjLrFuFV76rf1burpYKSG5l9n1-894O8TLIWRv2a3M97fdjhT1HFxBCjq39ZUEPyNPosNxn137-KxjPqTFtJT0SX6IY-VMGUjr--sSTJyRmB_b4jRHp2UN39-ZslpQxeDHX6QecGJ7OKjQ2KTXVrLgzhn8764cc8ksywAAjUlt63yJp2B-r3czDDMUSGUz91NPFQe7mlmQCBFoqanZnn1GHsQRyNIWOiYElnNH_cOyeor681BiTopVgEteqhwv0QA0UThNbi0CIhTJLJwiPHgL9nTgqJOwbo1IWSKTPRusM30XOwXGKZ0AeC7_JEXGGrCrxgfm9JF9IHj8thvczGGQW3Mcx5h47FwNCirE3dqSwidhpyA13D25Qk0TX2Yj7r0imdzxYzziKvEEeuwdx1jTdTa9QKoFL_EQ1mv4s8KUyCCTyrZvB3sZUmseN8YheEr8xBR-572VQTkxEtO59cNX4tksS0gRqsxCszEffDXEvae3elv6g-iYSaK4oXUiF2pmvW44Ez2HHjbXAZHyDYYxSFEieCR_8ByuNadGDA6FfFxwZIWujG9AkjEW34nEhS5-vFM9yBh-7RaoisjG_WpTBGg2xKrjBLg9p18JzpS8ojCKxi24I4nPZd1m2K7OK7E=w1255-h738" width="600"/>
    </div> 
</div>

#### 이스케이프 프롬 타르코프
<div style="display:flex"> <!-- 타르코프 -->
    <!-- 원본 -->
    <div style="flex:1;padding-right:10px;">
        <img src="https://lh3.googleusercontent.com/fife/AAbDypCYhh8TraJwwVMFTG7kXI8bmuF8Hm6waf6kXz4opzyBD2Xybi5gjbvW2NRKJp2MOYZ-dGdawQZ-JvdN3ZMWaFIhKUlwmEcosUV7hzFgoAe2Lf8OO0I9vwI6sUaeOwaZRU5fB5cPlWS4jW7vZWBaZWdLHK7btepzi4BDJo1FB3Nciaji8gdEcLBJ6KscT3k8MbrrQbNo9pkz18rkFt-7s1GPEVrHgrB7acJqfbqEqefzsMK58A9Dg40jM_Yn3QW1vhlnlFo3tEB_2lf86n9YryylSYBEkKREozhmrkV0P7Hc9qyxCY6mul9BPuunaYl3-NVLrXBvSYfsgpAAub9G5JPaihf2tJQLCyX7k6PSknwBfm43pMualXpn95jLtdLL-fIy04OYZLquttQqpvfGkBjmi_Te645TVixFoTXwVWNyGoOZjDVrOjxdIh5HblJiLYTy7GlyKFFb-VU3FqxB5ORN3jRd_v0Vv7OH2CgiGtX2pt7AxZK8OHmoi-_M-5G7BkRj2w6G7FJBHbR2pDDN04CwguKr_Yh59eB1JHWfoicVOqazT0xVAh3uohQy45zwz2BRK55E5tAKH1JhETWwScHJDqftV9FoEmWe8DjUcnfrxCPR4cFaromOKZZpD1o1XjY5o3XN_wZTZFLug4NyynLsYcnIaEfArn9auE-aJPrPcBe_3pI4fquWpyIOEI4P4MHj_L5dc6ctFgQbqZWeoSRhkR_1_PWo-aXEyXWDnLT0nJKvutjxYmUpFn40qfmln-yIfMnC2pibw80eEseyAahflYIXnwOEmsOvArik7ePwSJoJQchCmZD_Z3v-cZVs40rqG1Y7WvmbHRylAziU5_r7DzPqlWhcO9LMMUfWldIOuYXM62pbsm0lmy7SEMZT37ZFoh6zjEemLPScaCvD21ciwCaPE6YZP0afsye_uRliufSJa85sYbzp5MrLczgA2cqu_OuaO8YFIph_kuVAGFj8vSBHm4mPxz1E3lDUoTYD41JJQKwE_srxyzgR3Db0LUYtus7YYt6_WgI5GnKLPXV24x1kmjWWU8MlK-R0x-DWmZNTihC4I3Mjm27B9Y_NbAlB0Es8RoddzurhkC7YIXA4Nj7F-FUOKV2qRr0NoJ-1dVxk2AJZO7wzCa9ClK2Q48SfveTbvgRdiP5BbNkgRHPZRgdP_NqipI9PPGzaLL2d4Rf8uxp34T4EhMldfAq_PPVmnOnb-80-DgbSdwqa2JpSoeo62znwDwy_NrMBr8YX88Xe__goPex6n82W-RQpjz6eHSsjjXvQUB3iZjipTCW1HH2Gjgh5BgjKKuOzTTVAksr1s-AcYnhfTtwfTxKJGjx-4l-IVPQXF7RsCDcl9FsA8uul1LSWGMSdWNDMpTxgAqo3cEhR3rc5l9qLy1IzLGblC2yiAU-rtypAJBLEO9zLpPlH3yxabnRBDuIoFzfTJLLNWeRCns2wVyFQezhuvNdfguX5_KfaMqZPFr-mbiMikr4pG3c8_sqdzsZtnjvE6YXSCOOBQ6AxacM=w518-h738" width="600"/>
    </div>
    <!-- 구현 -->
    <div style="flex:1;padding-left:10px;">
        <img src="https://lh3.googleusercontent.com/fife/AAbDypA0V_nsvM0M-Bw3nhsitdaqJJURvGORVgw6_R7G1FFAbjAslGxU4f709HJ3S-gkSkQOD7uCOK1Y70LhhrS6M_z_E37aqbsJiZk6MFrSnFeH8MIzxATo9-fybnol84Sqa6btGbR7i0T1-FSHs7pV0lYM3v5eYpsnmGHgvp4fuBZAdomx9Dnz3Zn9loOYHGJgxlI_o-zYVWfDu7LbKmt_-I10oo3U5fk28KK9RejPqMyZzRrWp5psrETaDwlyohhXT24ukwCPUn1LrdQgOdZKSWwQojt4w3XB3k1hqovILC0vQjBl6kwdbiAsGL4v8JTjh5BbcJ9Xhxa20ZnFGOkAauXkVryDbAUxOI_6iL1Szs1sN75VU8YFcSiV5_A57cXYX-ad_Kit98ADE2NAmTSm_FuU2pVK9udgd6f7X0dGeswnbcvqgEz1JujpGvVD2IKp1-hq5w452-QfiHqOCAEZzJv6b6M7lm0hALuh_YsnCRB28PCEuhMa-aAa4bvqeki1XB8eutrTkj_C1jxvB88QEImTpWIblgy-2tmVQeu8PQ0PS58Z_6s74ft_7yAuNMqdgrHcuEGrB_9l-KJs1WcZ9QuIygJv-EUPIO2GUIsNr6u_tgTY5ioZVWrQtrh6Qlp-nP3Nm855AxQ8HWOXhfIg0v53mhgaRROKWL9A5r3-Hhbup7o6f_AicicXxS17nLqifmByr2Bd4n7DBrqlGsoDTM8AW9lhRtNmoyqvsl52mU4SMkLmJuHMTXAAqGkJ9Z8WgytqrQII5aJlOYdy9Th_XZOu-QJReetdiKYPCCA5wY-9TpzscLOwwDoIJH_kmEVwXa1kBLKBk2Jv_KzRTKvsYGILSbpme3ogmF6oh2W7vUNlrmDwZE_xhXsrjKmknZc6JQFZHIQKg1fGZpt6NNHFR7u_GK5Gw6LKmlpJerAf68AklFWJj5fPrDXDjWUJp_5QPfJFzWKrwzsZVd6jlLZ2vDCgXdVT8Tz8HMxTMOZp0Z8XLFhk9LH9RFnB40SK0ll0fw6GK5l0GVJ0uydSKNhvP0PZbs3goRPslrE8HwSR0XIPdXombEmYroL3iqE2Qey6wOT52VvExDIGQIm5N1eYTLS9vS5XMWtDtfF23WQQaDGlr-X1s8jyb3mI6hSIwmdZtqkvCFCcGsUERXujw8H4z6gU3vnXvD54x1p-j-zurt0omeSW_kkw5AZfO6qnHhdOSYDvaQ7LzfokKRY5s5GK8f5Qvrvu1Rn-oeeBbORXovdzIvEh4O9EuI1sD5IxWYSM948cv-plg22bWXoGtqlBkLNTy4SSKA57NZdctMu8oOErJQTKrqOKGY3vy9wtmD4rRk586oSOt8QRvobOvcIJmFPASAV-vBZZDJZxM7d17KQVLrA5lLfegxNCrH6wakWdHlZcg8_7_eFIpbjlyRQIsgBX4QaKrmDg_RHoDFs466AnAjhxvWE3s4pOzEp_DKZeZbIxIn3pikFyKuAVE8AaLt-hrxUHc7dV7QLThLCmvsJdDIiO5rnZqAEiRrc=w1255-h738" width="600"/>
    </div> 
    
</div>

[원본 영상 출처](http://youtube.com/watch?t=227&v=IBG44eHL2xg&feature=youtu.be&ab_channel=SirHansVaderEN)    

##     
##        
  ### 참고
  ---
시연 영상 : [유튜브(정리중)]()       
실행 파일 : [구글 드라이브](https://drive.google.com/file/d/12ix8i4Nq4FSzWUAIyZiHRLR5q88buyeV/view?usp=sharing)      
