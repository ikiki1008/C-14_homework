#include <iostream>
#include <queue>
#include <vector>
#include <functional>
#include <unordered_map>
#include <future>  

//�� ��带 ��Ÿ����, Functor�� �����ϰ� input, output�� �����մϴ�
struct Node {
    //��忡�� ������ �Լ��� ���� (�Է��� �޾� ��°��� ��ȯ)
    std::function<int(std::vector<int>)> func;

    //��尡 �ʿ��� �Է°��� ����    
    int inputs_needed;

    //���޵� �Է°��� ������ ����
    std::vector<int> inputs;

    //���� ����� ���� ��°��� ������ ����
    std::vector<int> outputs;

    //�����ؾ� �� ���� ������ ����
    std::vector<Node*> next_nodes;

    Node(std::function<int(std::vector<int>)> f, int n)
        : func(f), inputs_needed(n) {}

    void add_input(int value) {
        inputs.push_back(value);  // �Է°��� inputs ���Ϳ� �߰�
    }

    //�Է��� ��� �����Ǿ��� �� ����
    void execute() {
        if (inputs.size() == inputs_needed) {
            int output = func(inputs);
            outputs.push_back(output);

            //����� ���� ����� ���� ���鿡�� ����
            for (auto* next : next_nodes) {
                next->add_input(output);  //���� ��忡 ��°��� �Է����� �߰�
            }
        }
    }
};

int main() {
    // �� ��� ����
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

    //�ʱ��Է°� ����
    A.add_input(1);
    B.add_input(2);

    //BFS ����
    std::queue<Node*> q;
    q.push(&A);
    q.push(&B);

    // BFS�� �׷��� Ž�� �� ���� ó�� �غ�
    std::vector<std::future<void>> futures;

    // ť�� �� ������ ����ؼ� �����մϴ�.
    while (!q.empty()) {
        Node* node = q.front();
        q.pop();

        // ���� ������ �����ϵ��� async�� �����մϴ�.
        // async�� ����� ��带 �񵿱�� �����ϰ� �Լ��� ������ ���ķ� ó���մϴ�.
        futures.push_back(std::async(std::launch::async, [node]() { node->execute(); }));

        for (Node* next : node->next_nodes) {
            if (next->inputs.size() == next->inputs_needed) {
                q.push(next);
            }
        }
    }

    for (auto& f : futures) {
        f.get();  //�۾��� ���� ������ ���
    }

    return 0;
}