#include "TextBlock.h"

void TextBlock::UpdateText() {
	m_tx.setFillColor(m_fillColor);

	m_tx.setString(m_str);
	m_tx.setCharacterSize(m_charectSize);
	if (m_lenght != m_tx.getGlobalBounds().width) {
		m_reallyCharectoeSize = ((m_tx.getGlobalBounds().width-m_lenght)+m_reallyCharectoeSize)/2;
		m_lenght = m_tx.getGlobalBounds().width;
	}

	float spacing = m_font.getLineSpacing(m_charectSize);

	float characterSize = m_charectSize;
	std::string str = m_str;
	int maxCharectCount = std::floor(m_transform.Size().x / m_reallyCharectoeSize);

	if ((m_charectSize)* m_str.size() > m_transform.Size().x) {
		switch (m_outOfBoundsType) {
		case TextOutOfBoundType::Scale:
			characterSize = m_transform.Size().x / m_str.size();
			m_tx.setCharacterSize(characterSize);
			break;
		case TextOutOfBoundType::Scroll:
			str = m_str.substr(m_str.size() - maxCharectCount - 1, m_str.size() - 1);
			break;
		case TextOutOfBoundType::NextLine:
			for (int i = maxCharectCount; i < std::count_if(m_str.begin(),m_str.end(),[](char a){return a!='\n';}); i += maxCharectCount) {
				str.insert(str.begin() + i, '\n');
			}
			break;
		}
	}

	switch (m_horizontalSpaceType) {
	case TextHorizontalSpaceType::Centre:
		m_offset.x = (m_transform.Size().x/2 - ((Clamp(m_str.size(), 0, maxCharectCount) + 1)* (m_reallyCharectoeSize))/2.1);
			break;
	case TextHorizontalSpaceType::Left:
		m_offset.x = m_tx.getLineSpacing();
		break;
	case TextHorizontalSpaceType::Right:
		m_offset.x = (m_transform.Size().x - ((Clamp(m_str.size(), 0, maxCharectCount) + 1) * (m_reallyCharectoeSize)));
		break;
	}

	switch (m_verticalSpaceType) {
	case TextVerticalSpaceType::Centre:
		m_offset.y = m_transform.Size().y / 1.7 - m_charectSize;
		break;
	case TextVerticalSpaceType::Top:
		m_offset.y = 0;
		break;
	case TextVerticalSpaceType::Bottom:
		m_offset.y = m_transform.Size().y-m_charectSize;
		break;
	}

	m_tx.setCharacterSize(characterSize);
	m_tx.setString(str);
}