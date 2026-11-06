
#include <iostream>
#include <stdexcept>
using namespace std;

class Node
{

public:
    explicit Node(float alpha = 1.0);
    bool setAlpha(float alpha);
    float getAlpha() const;

private:
    float alpha_;
};

// contructor
Node::Node(float alpha) : alpha_{1.0f}
{
    if (!setAlpha(alpha))
    {
        throw invalid_argument("Invalid alpha value. Must be between 0 and 1.");
    }
}

bool Node::setAlpha(float alpha)
{
    if (alpha >= 0.0 && alpha <= 1.0)
    {
        alpha_ = alpha;
        return true;
    }
    return false;
}

float Node::getAlpha() const
{
    return alpha_;
}

int main()
{
    Node title{0.5f};
    Node icon;

    cout << "Title alpha: " << title.getAlpha() << std::endl;
    cout << "Icon alpha: " << icon.getAlpha() << std::endl;

    bool accepted = icon.setAlpha(2.0);

    cout << "Icon alpha set to 2.0: " << (accepted ? "Accepted" : "Rejected") << std::endl;
    cout << "Title alpha : " << title.getAlpha() << std::endl;
}