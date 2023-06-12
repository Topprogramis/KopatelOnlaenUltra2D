#pragma once
#include"../include.h"
#include"../Action.h"

class Animator;

__interface IAnimatable {
public:
	bool UpdateAnimateParm(std::string name, glm::vec4 value);
};

class AnimationClip {
public:
	enum class State {
		Play,
		Pause,
		End
	};

	AnimationClip(std::string animateParm, float duration, float speed, bool isLoop, bool PlayOnAwake = true) {
		m_duration = duration;
		m_speed = speed;

		Loop(isLoop);

		m_animParm = animateParm;

		if (PlayOnAwake) {
			Play();
		}
		else {
			Pause();
		}
	}

	void Loop(bool loop) {
		m_isLoop = loop;
	}
	bool isLoop() {
		return m_isLoop;
	}

	virtual glm::vec4 getValue() { return {}; };

	virtual void Process() {};
	
	virtual void Reset() {};

	void Play() {
		m_state = State::Play;
	}
	void Pause() {
		m_state = State::Pause;
	}
	void End() {
		m_state = State::End;
		OnEnd.Invoke(this);
	}

	State getState() {
		return m_state;
	}

	std::string getAnimateParm() {
		return m_animParm;
	}

	int fixEndInd;
	Action<AnimationClip*> OnEnd;

protected:
	float m_duration,m_speed;
	float m_donePocent;

	bool m_isLoop = false;

	std::string m_animParm;

	State m_state = State::Pause;
};

class FloatAnimationClip : public AnimationClip {
public:
	FloatAnimationClip(std::string animateParm, float duration, float speed, bool isLoop, float startValue, float targetValue) : AnimationClip(animateParm, duration, speed, isLoop) {
		m_startValue = startValue;
		m_targetValue = targetValue;

		m_step = (targetValue - startValue) / duration * speed * 0.02f;

		Reset();

		Play();
	}

	void Reset() override {
		m_currentValue = m_startValue;
		OnEnd = Action<AnimationClip*>();
		Play();
	}
	void Process() override {
		if (std::abs(m_targetValue-m_currentValue)<= std::abs(m_step)) {
			if (isLoop())
				Reset();
			else {
				End();
			}
		}
		else if(m_state == State::Play) {
			m_currentValue += m_step;
		}
	}

	glm::vec4 getValue()  {
		return { m_currentValue,0,0,0 };
	}


private:
	float m_startValue, m_targetValue;

	float m_step = 0.0f;

	float m_currentValue;
};

class Vec2AnimationClip : public AnimationClip {
public:
	Vec2AnimationClip(std::string animateParm, float duration, float speed, bool isLoop, glm::vec2 startValue, glm::vec2 targetValue) : AnimationClip(animateParm, duration, speed, isLoop) {
		m_startValue = startValue;
		m_targetValue = targetValue;

		m_step = (targetValue - startValue) / duration * speed * 0.02f;

		Reset();

		Play();
	}

	void Reset() override {
		m_currentValue = m_startValue;
		OnEnd = Action<AnimationClip*>();
		Play();
	}
	void Process() override {
		if (std::abs(m_targetValue.x - m_currentValue.x) <= std::abs(m_step.x) &&
			std::abs(m_targetValue.y - m_currentValue.y) <= std::abs(m_step.y)) {
			if (isLoop())
				Reset();
			else {
				End();
			}
		}
		else if (m_state == State::Play) {
			m_currentValue += m_step;
		}
	}

	glm::vec4 getValue() {
		return { m_currentValue,0,0 };
	}


private:
	glm::vec2 m_startValue, m_targetValue;

	glm::vec2  m_currentValue;

	glm::vec2 m_step;
};
class Vec3AnimationClip : public AnimationClip {
public:
	Vec3AnimationClip(std::string animateParm, float duration, float speed, bool isLoop, glm::vec3 startValue, glm::vec3 targetValue) : AnimationClip(animateParm, duration, speed, isLoop) {
		m_startValue = startValue;
		m_targetValue = targetValue;

		m_step = (targetValue - startValue) / duration * speed * 0.02f;

		Reset();

		Play();
	}

	void Reset() override {
		m_currentValue = m_startValue;
		OnEnd = Action<AnimationClip*>();
		Play();
	}
	void Process() override {
		if (std::abs(m_targetValue.x - m_currentValue.x) <= std::abs(m_step.x) &&
			std::abs(m_targetValue.y - m_currentValue.y) <= std::abs(m_step.y) &&
			std::abs(m_targetValue.z - m_currentValue.z) <= std::abs(m_step.z)){
			if (isLoop())
				Reset();
			else {
				End();
			}
		}
		else if (m_state == State::Play) {
			m_currentValue += m_step;
		}
	}

	glm::vec4 getValue() {
		return { m_currentValue,0 };
	}


private:
	glm::vec3 m_startValue, m_targetValue;

	glm::vec3  m_currentValue;

	glm::vec3 m_step;
};
class Vec4AnimationClip : public AnimationClip {
public:
	Vec4AnimationClip(std::string animateParm, float duration, float speed, bool isLoop, glm::vec4 startValue, glm::vec4 targetValue) : AnimationClip(animateParm, duration, speed, isLoop) {
		m_startValue = startValue;
		m_targetValue = targetValue;

		m_step = (targetValue - startValue) / duration * speed * 0.02f;

		Reset();

		Play();
	}

	void Reset() override {
		m_currentValue = m_startValue;
		OnEnd = Action<AnimationClip*>();
		Play();
	}
	void Process() override {
		if (std::abs(m_targetValue.x - m_currentValue.x) <= std::abs(m_step.x) &&
			std::abs(m_targetValue.y - m_currentValue.y) <= std::abs(m_step.y) &&
			std::abs(m_targetValue.z - m_currentValue.z) <= std::abs(m_step.z) &&
			std::abs(m_targetValue.w - m_currentValue.w) <= std::abs(m_step.w)) {
			if (isLoop())
				Reset();
			else {
				End();
			}
		}
		else if (m_state == State::Play) {
			m_currentValue += m_step;
		}
	}

	glm::vec4 getValue() {
		return m_currentValue;
	}


private:
	glm::vec4 m_startValue, m_targetValue;

	glm::vec4  m_currentValue;

	glm::vec4 m_step;
};

struct Lexem {
	std::string m_name;

	Action<std::vector<std::string>> m_func;
	std::vector<std::string> args;
};

class Animator {
public:
	Animator(IAnimatable* animateObject);

	void Process() {
		if (m_animateObject != nullptr) {
			for (auto& i : m_playings) {
				i->Process();

				m_animateObject->UpdateAnimateParm(i->getAnimateParm(), i->getValue());
			}

			if (m_runScript.size() == 0)
				return;

			
			if (m_waitingEnd.size() != 0) {
				return;
			}

			if (m_waitTime > 0) {
				m_waitTime -= 0.02f;
				return;
			}


			CompileNext();
		}
	}

	void AddAnimClip(std::string name, AnimationClip* clip) {
		m_clips[name] = clip;
	}
	void AddActionForEmit(std::string name, Action<>* action) {
		m_actionsForEmit[name] = action;
	}

	void AddScriptFormStr(std::string name, std::string text) {
		auto lexems = ParseLexems(text);
		m_scripts[name]=lexems;
	}
	void AddScriptFormFile(std::string name, std::string path) {
		std::string line;

		std::string text;

		std::ifstream script(path);
		if (script.is_open())
		{
			while (std::getline(script, line))
			{
				text += line;
			}
		}
		script.close();

		m_scripts[name] = ParseLexems(text);
	}

	void SelectScript(std::string name) {
		m_runScript = m_scripts[name];
		m_currentLexem = 0;
	}

	std::map<std::string, bool> variabls = {};

private:
	IAnimatable* m_animateObject;

	float m_waitTime = 0.0f;

	std::map<std::string, Action<>*> m_actionsForEmit = {};
	std::map<std::string, AnimationClip*> m_clips = {};

	std::vector<AnimationClip*> m_playings = {};
	std::vector<AnimationClip*> m_waitingEnd = {};


	std::map<std::string, std::vector<Lexem>> m_scripts = {};

	std::vector<Lexem> m_runScript = {};
	int m_currentLexem = 0;


	void AnimationEnd(AnimationClip* animation) {
		auto playingIter = std::find(m_playings.begin(), m_playings.end(), animation);
		auto waitIter = std::find(m_waitingEnd.begin(), m_waitingEnd.end(), animation);

		if (waitIter != m_waitingEnd.end()) {
			m_waitingEnd.erase(waitIter);
		}
		if (playingIter != m_playings.end()) {
			m_playings.erase(playingIter);
		}

		//		animation->OnEnd.PopListener(animation->fixEndInd);
	}

	void PlaySc(std::vector<std::string> args) {
		for (auto& clipName : args) {

			if (m_clips[clipName] == nullptr) {
				std::cout << "Anim clip is not found" << std::endl;
				continue;
			}

			m_playings.push_back(m_clips[clipName]);
			m_clips[clipName]->Reset();

			m_clips[clipName]->fixEndInd = m_clips[clipName]->OnEnd.AddListener(std::bind(&Animator::AnimationEnd, this, std::placeholders::_1));
		}
	}
	void SelectNewScript(std::vector<std::string> args) {
		m_runScript = m_scripts[args[0]];
		m_currentLexem = 0;
	}
	void WaitWhileEnd(std::vector<std::string> args) {
		if (args[0] == "" && m_playings.size() != 0) {
			m_waitingEnd.push_back(*(m_playings.end() - 1));
			return;
		}
		for (auto& clipName : args) {

			if (m_clips[clipName] == nullptr) {
				std::cout << "Anim clip is not found" << std::endl;
				continue;
			}

			auto iter = std::find(m_playings.begin(), m_playings.end(), m_clips[clipName]);
			if (iter != m_playings.end()) {
				m_waitingEnd.push_back(m_clips[clipName]);
			}
		}
	}
	void waitFor(std::vector<std::string> args) {
		float time = std::stof(args[0]);

		m_waitTime += time;
	}
	void waitWhile(std::vector<std::string> args) {
		std::string varName = args[0];
	}
	void playIf(std::vector<std::string> args) {
		std::string animName = args[0];
		std::string varName = args[1];

		if (variabls[varName]) {
			PlaySc({ animName });
		}
	}
	void Goto(std::vector<std::string> args) {
		int lineNum = std::stoi(args[0]);

		if (lineNum >=  m_runScript.size()) {
			std::cout << "goto is linked on line that does not contain in script" << std::endl;
			return;
		}
		m_currentLexem = lineNum;
	}
	void Emit(std::vector<std::string> args) {
		std::string actionName = args[0];

		if (m_actionsForEmit.find(actionName) != m_actionsForEmit.cend()) {
			m_actionsForEmit[actionName]->Invoke();
		}
	}

	//m_fucntions

	std::vector<Lexem> ParseLexems(std::string scriptText) {
		std::vector<std::string> m_commands = {};

		std::string m_currentComand = "";
		for (auto& i : scriptText) {
			if (i == ';') {
				m_commands.push_back(m_currentComand);

				m_currentComand = "";
			}
			else {
				m_currentComand += i;
			}
		}


		std::vector<Lexem> currentScript = {};

		for (auto& command : m_commands) {
			std::string name;
			std::vector<std::string> args = { "" };

			if (*(command.end() - 1) != ')') {
				std::cout << "syntax error" << std::endl;
				return {};
			}

			bool inSkob = false;
			for (int chP = 0; chP < command.size(); chP++) {
				if (inSkob) {
					if (command[chP] == ',') {
						args.push_back("");
					}
					else if (chP != command.size() - 1) {
						args[args.size() - 1] += command[chP];
					}
				}
				else if (command[chP] == '(') {
					inSkob = true;
					continue;
				}
				else {
					name += command[chP];
				}
			}

			Lexem lx;

			lx.args = args;
			lx.m_name = name;

			if (name == "Play") {
				lx.m_func.AddListener(std::bind(&Animator::PlaySc, this, std::placeholders::_1));
			}
			else if (name == "WaitFor") {
				lx.m_func.AddListener(std::bind(&Animator::waitFor, this, std::placeholders::_1));
			}
			else if (name == "WaitWhile") {
				lx.m_func.AddListener(std::bind(&Animator::waitWhile, this, std::placeholders::_1));
			}
			else if (name == "PlayIf") {
				lx.m_func.AddListener(std::bind(&Animator::playIf, this, std::placeholders::_1));
			}
			else if (name == "Goto") {
				lx.m_func.AddListener(std::bind(&Animator::Goto, this, std::placeholders::_1));
			}
			else if (name == "Emit") {
				lx.m_func.AddListener(std::bind(&Animator::Emit, this, std::placeholders::_1));
			}
			else if (name == "WaitWhileEnd") {
				lx.m_func.AddListener(std::bind(&Animator::WaitWhileEnd, this, std::placeholders::_1));
			}
			else if (name == "Call") {
				lx.m_func.AddListener(std::bind(&Animator::SelectNewScript, this, std::placeholders::_1));
			}

			currentScript.push_back(lx);
		}

		return currentScript;
	}

	void CompileNext() {
		if (m_runScript.size() == 0)
			return;
		 m_runScript[m_currentLexem].m_func.Invoke( m_runScript[m_currentLexem].args);

		if (m_currentLexem + 1 >=  m_runScript.size())
			m_currentLexem = 0;
		else
			m_currentLexem++;
	}


};