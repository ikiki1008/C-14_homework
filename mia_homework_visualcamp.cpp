#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <unordered_map>
#include <future>  

//각 노드를 나타내며, Functor를 저장하고 input, output을 관리합니다
struct Node {
    //노드에서 실행할 함수를 저장 (입력을 받아 출력값을 반환)
    std::function<int(std::vector<int>)> func;

    //노드가 필요한 입력값의 개수    
    int inputs_needed;

    //전달된 입력값을 저장할 벡터
    std::vector<int> inputs;

    //실행 결과로 얻은 출력값을 저장할 벡터
    std::vector<int> outputs;

    //실행해야 할 다음 노드들을 저장
    std::vector<Node*> next_nodes;

    Node(std::function<int(std::vector<int>)> f, int n)
        : func(f), inputs_needed(n) {}

    void add_input(int value) {
        inputs.push_back(value);  // 입력값을 inputs 벡터에 추가
    }

    //입력이 모두 충족되었을 때 실행
    void execute() {
        if (inputs.size() == inputs_needed) {
            int output = func(inputs);
            outputs.push_back(output);

            //노드의 실행 결과를 다음 노드들에게 전달
            for (auto* next : next_nodes) {
                next->add_input(output);  //다음 노드에 출력값을 입력으로 추가
            }
        }
    }
};

int main() {
    // 각 노드 정의
    Node A([](std::vector<int> inputs) { return inputs[0] + 1; }, 1);
    Node B([](std::vector<int> inputs) { return inputs[0] + 2; }, 1);
    Node C([](std::vector<int> inputs) { return inputs[0] + inputs[1]; }, 2);
    Node D([](std::vector<int> inputs) { return inputs[0] * 2; }, 1);
    Node E([](std::vector<int> inputs) { return inputs[0] * 3; }, 1);
    Node F([](std::vector<int> inputs) { return inputs[0] + inputs[1]; }, 2);

    A.next_nodes.push_back(&C);
    B.next_nodes.push_back(&C);
    B.next_nodes.push_back(&F);
    C.next_nodes.push_back(&D);
    C.next_nodes.push_back(&E);
    C.next_nodes.push_back(&F);

    //초기입력값 제공
    A.add_input(1);
    B.add_input(2);

    //BFS 생성
    std::queue<Node*> q;
    q.push(&A);
    q.push(&B);

    // BFS로 그래프 탐색 및 병렬 처리 준비
    std::vector<std::future<void>> futures;

    // 큐가 빌 때까지 계속해서 실행합니다.
    while (!q.empty()) {
        Node* node = q.front();
        q.pop();

        // 병렬 실행이 가능하도록 async로 실행합니다.
        // async를 사용해 노드를 비동기로 실행하고 함수의 실행을 병렬로 처리합니다.
        futures.push_back(std::async(std::launch::async, [node]() { node->execute(); }));

        for (Node* next : node->next_nodes) {
            if (next->inputs.size() == next->inputs_needed) {
                q.push(next);
            }
        }
    }

    for (auto& f : futures) {
        f.get();  //작업이 끝날 때까지 대기
    }

    return 0;
}