"""
[BFS - 너비 우선 탐색 (Breadth-First Search)]

문제 설명:
- BFS로 그래프를 탐색합니다.
- 가까운 정점부터 방문합니다.
- 큐(Queue)를 사용합니다.

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
BFS: [0, 1, 2, 3]


"""

from collections import deque

def bfs(graph, start):
    """
    너비 우선 탐색
    
    Args:
        graph: 그래프 딕셔너리
        start: 시작 정점
    
    Returns:
        방문 순서 리스트
    """
    visited = []
    
    queue = deque([start]) # 큐에 시작노드 삽입
    visited.append(start)

    while queue:
        v = queue.popleft() # v에 큐의 왼쪽 시작노드 추출
        for i in graph[v]: # 그래프의 v번 반복
            if i not in visited: # 방문록에 없다면 추가
                visited.append(i)
                queue.append(i)
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
    
    print("=== BFS (너비 우선 탐색) ===")
    result1 = bfs(graph,0)
    print(f"시작 정점: 0")
    print(f"방문 순서: {result1}")
