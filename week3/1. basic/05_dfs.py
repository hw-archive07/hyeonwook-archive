"""
[DFS - 깊이 우선 탐색 (Depth-First Search)]

문제 설명:
- DFS로 그래프를 탐색합니다.
- 깊이 방향으로 끝까지 탐색합니다.
- 재귀 또는 스택을 사용합니다.

입력:
- graph: 그래프 (인접 리스트)
- start: 시작 정점

출력:
- 방문 순서

예제:
그래프:
  0 ─── 1
  │     │
  └─ 2 ─┘
      │
      3

시작: 0
DFS: [0, 1, 2, 3] (순서는 구현에 따라 다를 수 있음)


"""

def dfs(graph, start, visited=None):    # dfs(graph, [1,2], visited)
    """
    깊이 우선 탐색 (재귀)
    
    Args:
        graph: 그래프 딕셔너리
        start: 현재 정점
        visited: 방문 리스트
    
    Returns:
        방문 순서 리스트
    """
    if visited == None:  # 초기화       #값이 있기때문에 초기화x
        visited = []
    
    visited.append(start)  # 시작 정점인 start를 추가   # 재귀호출로 받은 1, 2를 visited = [0,]에 추가 - [0, 1, ]
                                                      # 이러한 식으로 start가 재귀호출로 불러오는 값은
                                                         # 1 = [0(있음), 2(마지막 반복에서 추가)]
                                                         # 2 = [0(있음), 1(있음), 3(추가))]
                                                         # 3 = [2(있음)]
                                                         # return visited = [0, 1, 2, 3] 

    for i in graph.get(start):  # graph에서 정점에 관한 값을 가져옴 (예 : 0 이면 [1, 2] - 하지만 튜플, 리스트는 같은 취급으로 1만 들어감)
        if i not in visited:    # [1]가 visited에 없다면 재귀
            dfs(graph, i, visited) # dfs(graph, start=0, visited=None)에서 dfs(graph, [1], visited)로 바뀜
                


    return visited

# 테스트 케이스
if __name__ == "__main__":
    # 그래프 생성
    graph = {
        0: [1, 2],
        1: [0, 2],
        2: [0, 1, 3],
        3: [2]
    }
    
    print("=== DFS (깊이 우선 탐색) ===")
    result = dfs(graph, 0)
    print(f"시작 정점: 0")
    print(f"방문 순서: {result}")


