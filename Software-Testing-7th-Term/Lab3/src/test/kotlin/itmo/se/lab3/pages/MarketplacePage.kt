package itmo.se.lab3.pages

import itmo.se.lab3.pages.elements.ListingElement
import org.openqa.selenium.*
import org.openqa.selenium.support.FindBy
import org.openqa.selenium.support.PageFactory
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.WebDriverWait

class MarketplacePage(private val driver: WebDriver) {
    @FindBy(xpath = "//label[text()=\"Ключевые слова:\"]/following-sibling::input")
    lateinit var keywordsField: WebElement

    @FindBy(xpath = "//form[@name=\"content_form\"]/following-sibling::*/a[text()=\"Искать\"]")
    lateinit var searchButton: WebElement

    @FindBy(xpath = "//span[text()=\"Сортировка: \"]/following-sibling::a[text()=\"цена\"]")
    lateinit var sortByPriceToggle: WebElement

    init {
        driver["https://advego.com/shop/find/"]
        PageFactory.initElements(driver, this)
    }

    fun waitForItems() {
        val itemsLocator = By.xpath("//div[@class=\"list\"]/*")
        WebDriverWait(driver, 8).until(presenceOfElementLocated(itemsLocator))
    }

    fun listItems(): List<ListingElement> = ListingElement.findAll(driver)
}
