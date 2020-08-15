
#ifndef _ALITTLE_THREADCONSUMER_H_
#define _ALITTLE_THREADCONSUMER_H_

#include <thread>
#include <mutex>
#include <list>
#include <condition_variable>

namespace ALittle
{

template <typename T>
class ThreadConsumer
{
public:
    ThreadConsumer() : m_thread(nullptr), m_run(false)
    {
    }

    virtual ~ThreadConsumer()
    {
        Stop();
    }

protected:
    // ����
    void Start()
    {
        if (m_thread != nullptr) return;

        // ����Ϊ����ִ��
        m_run = true;
        // �����̣߳�����ʼִ��
        m_thread = new std::thread(ThreadConsumer::Run, this);
    }

    bool IsStart() const
    {
        return m_thread != nullptr;
    }

    // ���
    void Add(T& info)
    {
        if (m_thread == nullptr) return;

        // ����־��ӵ��б�
        std::unique_lock<std::mutex> lock(m_mutex);
        m_list.emplace_back(std::move(info));
        m_cv.notify_one();
    }

    // �ر�
    void Stop()
    {
        if (m_thread == nullptr) return;

        // �⿪��
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_run = false;
            m_cv.notify_all();
        }
        // �ȴ��߳���������
        m_thread->join();
        // ɾ���̶߳���
        delete m_thread;
        m_thread = nullptr;

        // ��ʼִ��
        while (!m_list.empty())
        {
            Flush(m_list.front());
            m_list.pop_front();
        }
    }

private:
    // ֧�̺߳���
    static int Run(ThreadConsumer* self)
    {
        // ������ʱ�б�
        std::list<T> temp_list;
        while (self->m_run)
        {
            // ��ȡִ�ж���
            {
                std::unique_lock<std::mutex> lock(self->m_mutex);
                while (self->m_run && self->m_list.empty())
                    self->m_cv.wait(lock);
                temp_list.swap(self->m_list);
            }

            // ��ʼִ��
            while (!temp_list.empty())
            {
                self->Flush(temp_list.front());
                temp_list.pop_front();
            }
        }

        return 0;
    }

protected:
    // ִ����־
    virtual void Flush(T& info) = 0;

private:
    std::mutex m_mutex;                 // ������
    std::condition_variable m_cv;       // ��������

private:
    std::list<T> m_list;	// �ȴ���־���б�

private:
    volatile bool m_run;	// ֧�߳��Ƿ�����ִ��
    std::thread* m_thread;	// �̶߳���
};

} // ALittle

#endif // _ALITTLE_THREADCONSUMER_H_
