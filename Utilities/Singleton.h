/*
 * This is a lightweight implementaion of a singleton superclass. To make a 
 * class singleton, simply include this file, inherit from Singleton, and 
 * set the template parameter to your class, additionall the templated singleton
 * should be a friend class to your class if your classes constructor is private. 
 * e.g 
 * MyClass : public Singleton<MyClass> {
 *      template<typename MyClass>
 *		friend class Singleton;
 *	private:
 *	MyClass();
 * }
 *
 */

#ifndef Singleton_h
#define Singleton_h
namespace AD537x {
template<class T> class Singleton
{
private:
	inline static T* ptr = nullptr;

public:
	static T* getInstance() {
		if (!ptr) {
			ptr = new T;
		}

		return ptr;
	}
};
}  // namespace AD537x
#endif  // Singleton_h
