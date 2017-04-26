#include "multi_threaded_tests.h"

#include <vector>

static int const kTestNThreadsCount = 10000;
void* test_n_threads_fn(void* info) {
    ThreadInfo* ti = reinterpret_cast<ThreadInfo*>(info);

    for (int i = ti->tid; i < kTestNThreadsCount; i += ti->thread_count) {
        ti->bst->insert(i);
    }

    return nullptr;
}


bool test_n_threads(size_t n) {
    INIT_TEST;
    std::vector<pthread_t> threads;
    std::vector<ThreadInfo> thread_info;

    std::shared_ptr<BinarySearchTree> bst = std::make_shared<BST>();
    for(size_t i = 0; i < n; ++i) {
        thread_info.emplace_back(i, n, bst);
        threads.emplace_back();
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, test_n_threads_fn, &thread_info[i]);
    }

    for (auto& t : threads) {
        pthread_join(t, nullptr);
    }

    for (int i = 0; i < kTestNThreadsCount; ++i) {
        EXPECT(bst->contains(i));
    }
            
    EXIT_TEST;
}

bool test_1_threads() { return test_n_threads(1); }
bool test_2_threads() { return test_n_threads(2); }
bool test_4_threads() { return test_n_threads(4); }
bool test_8_threads() { return test_n_threads(8); }
bool test_16_threads() { return test_n_threads(16); }