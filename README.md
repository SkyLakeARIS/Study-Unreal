# shooting range
<div id="image-table"> <!-- 요약 -->
    <table>
	    <tr>
    	    <td style="padding:10px">
        	    <img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_summary.gif" width="900"/>
      	  </td>
        </tr>
    </table>
</div>
    
 졸업 작품용으로 만들어졌으나, 그냥 버리기는 아까운 면이 있어서 상당수의 코드를 갈아엎었습니다.     
졸업 작품용은 old branch에서 확인할 수 있습니다.    
현재 정상적으로 실행되는 코드는 main branch에서 확인할 수 있습니다.    
그 외, 간간히 작업하고 있는 코드는 dev branch에 올리고 있습니다.    
         
 ### 작품 설명     
 ---
 언리얼 엔진을 사용한 FPS 트레이닝 게임입니다.    
제작 목적은 "FPS 게임 입문자가 FPS 장르에 좀 더 쉽게 적응하기 위해서 기본적인 시스템을 연습할 수 있게 한다." 
    입니다.       
이런 트레이닝 게임 특성상 실제 게임에서의 괴리감을 줄이는 것이 중요하다고 생각하여      
유명한 게임 3가지를 선정해 시스템을 최대한 따라 해봤습니다.        
대상이 된 게임은 아래와 같습니다.    
  1. 배틀필드 4     
  2. 레인보우 식스 시즈     
  3. 이스케이프 프롬 타르코프     
      
선정된 각 게임들은 투사체, 히트스캔, 총알 발사 위치, 랜덤스프레드 여부 등 서로 겹치지 않는 구현을 가지기 때문에 적절하다고 판단했습니다.      

애니메이션의 경우 직접 만들었기 때문에 어색합니다.  
   

### 구현
---    
#### 배틀필드    
<div id="image-table"> <!--배틀필드 -->
    <table>
	    <tr>
         <!-- 원본 -->
    	    <td style="padding:10px">
        	    <img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_bf%20original.gif" width="500"/>
      	    </td>
          <!-- 구현 -->
            <td style="padding:10px">
            	<img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_bf%20implementation.gif" width="500"/>
            </td>
        </tr>
    </table>
</div>


#### 레인보우식스 시즈
<div id="image-table"> <!--레인보우식스 -->
    <table>
	    <tr>
         <!-- 원본 -->
    	    <td style="padding:10px">
        	    <img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_r6%20original.gif" width="500"/>
      	    </td>
          <!-- 구현 -->
            <td style="padding:10px">
            	<img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_r6%20implementation.gif" width="500"/>
            </td>
        </tr>
    </table>
</div>

#### 이스케이프 프롬 타르코프
<div id="image-table"> <!--타르코프 -->
    <table>
	    <tr>
         <!-- 원본 -->
    	    <td style="padding:10px">
        	    <img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_tarkov%20original.gif" width="500"/>
      	    </td>
          <!-- 구현 -->
            <td style="padding:10px">
            	<img src="https://github.com/SkyLakeARIS/shooting-range/blob/main/gifs/shooting-range_tarkov%20implementation.gif" width="500"/>
            </td>
        </tr>
    </table>
</div>

[원본 영상 출처](http://youtube.com/watch?t=227&v=IBG44eHL2xg&feature=youtu.be&ab_channel=SirHansVaderEN)    

### 게임 규칙
---    
 1. 90초의 제한 시간 내에 높은 명중률과 정확도를 기록합니다.    
 2. 맵은 근거리, 중거리, 장거리 타겟 유형에 고정된 타겟, 이동형 타겟이 있습니다. (총 6개)      
 3. 무기는 권총, 소총, 저격총 3가지가 존재합니다.    
 4. 조준경은 1배율, 2.5배율, 6배율이 존재합니다. 단, 권총은 1배율로 고정됩니다.   
 5. 게임 모드는 배틀필드4, 레인보우식스 시즈, 이스케이프 프롬 타르코프 3가지가 존재합니다.      
 6. 타겟은 플레이트형, 인간형 2가지가 존재합니다.    
 7. 인게임 설정(ESC)에서 감도를 설정할 수 있습니다.   


  ### 참고
  ---     
작업 기록 : [노션](https://skylakearis.notion.site/shooting-range-de1a42e83c1c4ab2b5650aeaecad6ea2)     
시연 영상 : [유튜브](https://youtu.be/zujnafqtzek)        
실행 파일 : [구글 드라이브](https://drive.google.com/file/d/12ix8i4Nq4FSzWUAIyZiHRLR5q88buyeV/view?usp=sharing)      
실행 파일(졸작) : [구글 드라이브](https://drive.google.com/file/d/17ltXn8je2Ny-4dfKRjk5yN_gmczoCYsO/view?usp=sharing)      